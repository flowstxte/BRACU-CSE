<?php
require_once 'db_connect.php';

header('Content-Type: application/json');

$action = isset($_REQUEST['action']) ? $_REQUEST['action'] : '';

// Get all quotes
if ($action === 'get_quotes') {
    $search = isset($_GET['search']) ? sanitize($_GET['search']) : '';
    $category = isset($_GET['category']) ? sanitize($_GET['category']) : '';
    $user_id = isset($_GET['user_id']) ? intval($_GET['user_id']) : 0;
    
    $sql = "SELECT q.*, u.username, u.profile_picture,
            (SELECT COUNT(*) FROM likes WHERE quote_id = q.quote_id) as likes_count,
            (SELECT COUNT(*) FROM comments WHERE quote_id = q.quote_id) as comments_count,
            (SELECT media_url FROM media WHERE quote_id = q.quote_id LIMIT 1) as image_url";
    
    if (isLoggedIn()) {
        $current_user_id = $_SESSION['user_id'];
        $sql .= ", (SELECT COUNT(*) FROM likes WHERE quote_id = q.quote_id AND user_id = $current_user_id) as user_liked,
                  (SELECT COUNT(*) FROM favorites WHERE quote_id = q.quote_id AND user_id = $current_user_id) as user_favorited";
    }
    
    $sql .= " FROM quotes q JOIN users u ON q.user_id = u.user_id WHERE 1=1";
    
    if ($search) {
        $sql .= " AND (q.quote_text LIKE '%$search%' OR q.author_name LIKE '%$search%' OR u.username LIKE '%$search%')";
    }
    
    if ($category) {
        $sql .= " AND q.category = '$category'";
    }
    
    if ($user_id) {
        $sql .= " AND q.user_id = $user_id";
    }
    
    $sql .= " ORDER BY q.date_added DESC";
    
    $result = $conn->query($sql);
    $quotes = [];
    
    while ($row = $result->fetch_assoc()) {
        $quotes[] = $row;
    }
    
    echo json_encode(['success' => true, 'quotes' => $quotes]);
}

// Get single quote
elseif ($action === 'get_quote') {
    $quote_id = intval($_GET['quote_id']);
    
    $sql = "SELECT q.*, u.username, u.profile_picture,
            (SELECT COUNT(*) FROM likes WHERE quote_id = q.quote_id) as likes_count,
            (SELECT media_url FROM media WHERE quote_id = q.quote_id LIMIT 1) as image_url";
    
    if (isLoggedIn()) {
        $current_user_id = $_SESSION['user_id'];
        $sql .= ", (SELECT COUNT(*) FROM likes WHERE quote_id = q.quote_id AND user_id = $current_user_id) as user_liked,
                  (SELECT COUNT(*) FROM favorites WHERE quote_id = q.quote_id AND user_id = $current_user_id) as user_favorited";
    }
    
    $sql .= " FROM quotes q JOIN users u ON q.user_id = u.user_id WHERE q.quote_id = $quote_id";
    
    $result = $conn->query($sql);
    
    if ($result->num_rows > 0) {
        echo json_encode(['success' => true, 'quote' => $result->fetch_assoc()]);
    } else {
        echo json_encode(['success' => false, 'message' => 'Quote not found']);
    }
}

// Create quote
elseif ($action === 'create_quote') {
    if (!isLoggedIn()) {
        echo json_encode(['success' => false, 'message' => 'Please login first']);
        exit();
    }
    
    $user_id = $_SESSION['user_id'];
    $quote_text = sanitize($_POST['quote_text']);
    $author_name = isset($_POST['author_name']) ? sanitize($_POST['author_name']) : 'self';
    $category = isset($_POST['category']) ? sanitize($_POST['category']) : '';
    $mood = isset($_POST['mood']) ? sanitize($_POST['mood']) : '';
    
    if (empty($quote_text)) {
        echo json_encode(['success' => false, 'message' => 'Quote text is required']);
        exit();
    }
    
    // Insert quote
    $stmt = $conn->prepare("INSERT INTO quotes (user_id, quote_text, author_name, category, mood) VALUES (?, ?, ?, ?, ?)");
    $stmt->bind_param("issss", $user_id, $quote_text, $author_name, $category, $mood);
    
    if ($stmt->execute()) {
        $quote_id = $conn->insert_id;
        
        // Handle image upload
        if (isset($_FILES['image']) && $_FILES['image']['error'] === 0) {
            $allowed = ['jpg', 'jpeg', 'png', 'gif'];
            $filename = $_FILES['image']['name'];
            $ext = strtolower(pathinfo($filename, PATHINFO_EXTENSION));
            
            if (in_array($ext, $allowed)) {
                $new_filename = 'quote_' . $quote_id . '_' . time() . '.' . $ext;
                $upload_path = 'media/uploads/' . $new_filename;
                
                if (move_uploaded_file($_FILES['image']['tmp_name'], $upload_path)) {
                    $stmt = $conn->prepare("INSERT INTO media (quote_id, media_url) VALUES (?, ?)");
                    $stmt->bind_param("is", $quote_id, $upload_path);
                    $stmt->execute();
                }
            }
        }
        
        echo json_encode(['success' => true, 'message' => 'Quote created successfully', 'quote_id' => $quote_id]);
    } else {
        echo json_encode(['success' => false, 'message' => 'Failed to create quote']);
    }
}

// Update quote
elseif ($action === 'update_quote') {
    if (!isLoggedIn()) {
        echo json_encode(['success' => false, 'message' => 'Please login first']);
        exit();
    }
    
    $quote_id = intval($_POST['quote_id']);
    $user_id = $_SESSION['user_id'];
    $quote_text = sanitize($_POST['quote_text']);
    $author_name = sanitize($_POST['author_name']);
    $category = sanitize($_POST['category']);
    $mood = sanitize($_POST['mood']);
    
    // Check ownership
    $stmt = $conn->prepare("SELECT user_id FROM quotes WHERE quote_id = ?");
    $stmt->bind_param("i", $quote_id);
    $stmt->execute();
    $result = $stmt->get_result();
    
    if ($result->num_rows === 0 || $result->fetch_assoc()['user_id'] != $user_id) {
        echo json_encode(['success' => false, 'message' => 'Unauthorized']);
        exit();
    }
    
    // Update quote
    $stmt = $conn->prepare("UPDATE quotes SET quote_text = ?, author_name = ?, category = ?, mood = ? WHERE quote_id = ?");
    $stmt->bind_param("ssssi", $quote_text, $author_name, $category, $mood, $quote_id);
    
    if ($stmt->execute()) {
        echo json_encode(['success' => true, 'message' => 'Quote updated successfully']);
    } else {
        echo json_encode(['success' => false, 'message' => 'Update failed']);
    }
}

// Delete quote
elseif ($action === 'delete_quote') {
    if (!isLoggedIn()) {
        echo json_encode(['success' => false, 'message' => 'Please login first']);
        exit();
    }
    
    $quote_id = intval($_POST['quote_id']);
    $user_id = $_SESSION['user_id'];
    
    // Check ownership
    $stmt = $conn->prepare("SELECT user_id FROM quotes WHERE quote_id = ?");
    $stmt->bind_param("i", $quote_id);
    $stmt->execute();
    $result = $stmt->get_result();
    
    if ($result->num_rows === 0 || $result->fetch_assoc()['user_id'] != $user_id) {
        echo json_encode(['success' => false, 'message' => 'Unauthorized']);
        exit();
    }
    
    // Delete quote (cascades to media, comments, likes, favorites)
    $stmt = $conn->prepare("DELETE FROM quotes WHERE quote_id = ?");
    $stmt->bind_param("i", $quote_id);
    
    if ($stmt->execute()) {
        echo json_encode(['success' => true, 'message' => 'Quote deleted successfully']);
    } else {
        echo json_encode(['success' => false, 'message' => 'Delete failed']);
    }
}

// Like/Unlike quote
elseif ($action === 'toggle_like') {
    if (!isLoggedIn()) {
        echo json_encode(['success' => false, 'message' => 'Please login first']);
        exit();
    }
    
    $quote_id = intval($_POST['quote_id']);
    $user_id = $_SESSION['user_id'];
    
    // Check if already liked
    $stmt = $conn->prepare("SELECT like_id FROM likes WHERE user_id = ? AND quote_id = ?");
    $stmt->bind_param("ii", $user_id, $quote_id);
    $stmt->execute();
    $result = $stmt->get_result();
    
    if ($result->num_rows > 0) {
        // Unlike
        $stmt = $conn->prepare("DELETE FROM likes WHERE user_id = ? AND quote_id = ?");
        $stmt->bind_param("ii", $user_id, $quote_id);
        $stmt->execute();
        echo json_encode(['success' => true, 'liked' => false]);
    } else {
        // Like
        $stmt = $conn->prepare("INSERT INTO likes (user_id, quote_id) VALUES (?, ?)");
        $stmt->bind_param("ii", $user_id, $quote_id);
        $stmt->execute();
        echo json_encode(['success' => true, 'liked' => true]);
    }
}

// Favorite/Unfavorite quote
elseif ($action === 'toggle_favorite') {
    if (!isLoggedIn()) {
        echo json_encode(['success' => false, 'message' => 'Please login first']);
        exit();
    }
    
    $quote_id = intval($_POST['quote_id']);
    $user_id = $_SESSION['user_id'];
    
    // Check if already favorited
    $stmt = $conn->prepare("SELECT fav_id FROM favorites WHERE user_id = ? AND quote_id = ?");
    $stmt->bind_param("ii", $user_id, $quote_id);
    $stmt->execute();
    $result = $stmt->get_result();
    
    if ($result->num_rows > 0) {
        // Unfavorite
        $stmt = $conn->prepare("DELETE FROM favorites WHERE user_id = ? AND quote_id = ?");
        $stmt->bind_param("ii", $user_id, $quote_id);
        $stmt->execute();
        echo json_encode(['success' => true, 'favorited' => false]);
    } else {
        // Favorite
        $stmt = $conn->prepare("INSERT INTO favorites (user_id, quote_id) VALUES (?, ?)");
        $stmt->bind_param("ii", $user_id, $quote_id);
        $stmt->execute();
        echo json_encode(['success' => true, 'favorited' => true]);
    }
}

// Get comments
elseif ($action === 'get_comments') {
    $quote_id = intval($_GET['quote_id']);
    
    $sql = "SELECT c.*, u.username, u.profile_picture 
            FROM comments c 
            JOIN users u ON c.user_id = u.user_id 
            WHERE c.quote_id = $quote_id 
            ORDER BY c.date_commented DESC";
    
    $result = $conn->query($sql);
    $comments = [];
    
    while ($row = $result->fetch_assoc()) {
        $comments[] = $row;
    }
    
    echo json_encode(['success' => true, 'comments' => $comments]);
}

// Add comment
elseif ($action === 'add_comment') {
    if (!isLoggedIn()) {
        echo json_encode(['success' => false, 'message' => 'Please login first']);
        exit();
    }
    
    $quote_id = intval($_POST['quote_id']);
    $user_id = $_SESSION['user_id'];
    $comment_text = sanitize($_POST['comment_text']);
    
    if (empty($comment_text)) {
        echo json_encode(['success' => false, 'message' => 'Comment cannot be empty']);
        exit();
    }
    
    $stmt = $conn->prepare("INSERT INTO comments (quote_id, user_id, comment_text) VALUES (?, ?, ?)");
    $stmt->bind_param("iis", $quote_id, $user_id, $comment_text);
    
    if ($stmt->execute()) {
        echo json_encode(['success' => true, 'message' => 'Comment added']);
    } else {
        echo json_encode(['success' => false, 'message' => 'Failed to add comment']);
    }
}

// Get favorites
elseif ($action === 'get_favorites') {
    if (!isLoggedIn()) {
        echo json_encode(['success' => false, 'message' => 'Please login first']);
        exit();
    }
    
    $user_id = $_SESSION['user_id'];
    
    $sql = "SELECT q.*, u.username, u.profile_picture,
            (SELECT COUNT(*) FROM likes WHERE quote_id = q.quote_id) as likes_count,
            (SELECT media_url FROM media WHERE quote_id = q.quote_id LIMIT 1) as image_url
            FROM quotes q 
            JOIN users u ON q.user_id = u.user_id
            JOIN favorites f ON q.quote_id = f.quote_id
            WHERE f.user_id = $user_id
            ORDER BY f.date_favorited DESC";
    
    $result = $conn->query($sql);
    $quotes = [];
    
    while ($row = $result->fetch_assoc()) {
        $quotes[] = $row;
    }
    
    echo json_encode(['success' => true, 'quotes' => $quotes]);
}

elseif ($action === 'get_quote_of_the_day') {
    $sql = "SELECT q.*, u.username, u.profile_picture,
            (SELECT COUNT(*) FROM likes WHERE quote_id = q.quote_id) as likes_count,
            (SELECT COUNT(*) FROM comments WHERE quote_id = q.quote_id) as comments_count,
            (SELECT media_url FROM media WHERE quote_id = q.quote_id LIMIT 1) as image_url
            FROM quotes q
            JOIN users u ON q.user_id = u.user_id
            ORDER BY likes_count DESC, q.date_added DESC
            LIMIT 1";
    $result = $conn->query($sql);
    if ($result && $result->num_rows > 0) {
        echo json_encode(['success' => true, 'quote' => $result->fetch_assoc()]);
    } else {
        echo json_encode(['success' => true, 'quote' => null]);
    }
}

else {
    echo json_encode(['success' => false, 'message' => 'Invalid action']);
}
?>
