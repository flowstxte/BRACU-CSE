<?php
require_once 'db_connect.php';

// Redirect if not logged in
if (!isLoggedIn()) {
    redirect('login.php');
}

$currentUser = getCurrentUser();

// Get user stats
$user_id = $currentUser['user_id'];
$stmt = $conn->prepare("SELECT 
    (SELECT COUNT(*) FROM quotes WHERE user_id = ?) as quote_count,
    (SELECT COUNT(*) FROM favorites WHERE user_id = ?) as favorite_count,
    (SELECT COUNT(*) FROM likes l JOIN quotes q ON l.quote_id = q.quote_id WHERE q.user_id = ?) as likes_received");
$stmt->bind_param("iii", $user_id, $user_id, $user_id);
$stmt->execute();
$stats = $stmt->get_result()->fetch_assoc();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title><?php echo htmlspecialchars($currentUser['username']); ?>'s Profile - Quote Diary</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <!-- Navigation -->
    <nav class="navbar">
        <div class="nav-content">
            <a href="index.php" class="logo">Quote Diary</a>
            
            <button class="menu-toggle">☰</button>
            
            <ul class="nav-links">
                <li><a href="index.php">Home</a></li>
                <li><a href="profile.php">Profile</a></li>
                <li><a href="#" onclick="openModal('createQuoteModal')">New Quote</a></li>
                <li><a href="#" onclick="logout()">Logout</a></li>
                <li><button class="theme-toggle">Dark Mode</button></li>
            </ul>
        </div>
    </nav>

    <!-- Main Content -->
    <div class="container" style="margin-top: 2rem;">
        <!-- Profile Header -->
        <div class="profile-header">
            <img src="<?php echo htmlspecialchars($currentUser['profile_picture']); ?>" alt="Profile" class="profile-avatar">
            <h1><?php echo htmlspecialchars($currentUser['username']); ?></h1>
            <p style="color: var(--text-secondary);">
                Member since <?php echo date('F Y', strtotime($currentUser['date_joined'])); ?>
            </p>
            <button class="btn btn-secondary mt-1" onclick="openModal('editProfileModal')">Edit Profile</button>
        </div>

        <!-- Stats -->
        <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 1rem; margin-bottom: 2rem;">
            <div class="card text-center">
                <h2><?php echo $stats['quote_count']; ?></h2>
                <p style="color: var(--text-secondary);">Quotes Posted</p>
            </div>
            <div class="card text-center">
                <h2><?php echo $stats['favorite_count']; ?></h2>
                <p style="color: var(--text-secondary);">Favorites</p>
            </div>
            <div class="card text-center">
                <h2><?php echo $stats['likes_received']; ?></h2>
                <p style="color: var(--text-secondary);">Likes Received</p>
            </div>
        </div>

        <!-- Profile Tabs -->
        <div class="profile-tabs">
            <button class="tab active" onclick="switchTab('my-quotes')">My Quotes</button>
            <button class="tab" onclick="switchTab('favorites')">Favorites</button>
        </div>

        <!-- Tab Content -->
        <div id="my-quotes" class="tab-content">
            <div class="quotes-grid" id="myQuotesContainer">
                <div class="spinner"></div>
            </div>
        </div>

        <div id="favorites" class="tab-content hidden">
            <div class="quotes-grid" id="favoritesContainer">
                <div class="spinner"></div>
            </div>
        </div>
    </div>

    <!-- Edit Profile Modal -->
    <div class="modal" id="editProfileModal">
        <div class="modal-content">
            <div class="modal-header">
                <h2>Edit Profile</h2>
                <button class="close-modal" onclick="closeModal('editProfileModal')">×</button>
            </div>
            <form id="editProfileForm" onsubmit="handleUpdateProfile(event)">
                <div class="form-group">
                    <label>Username</label>
                    <input type="text" class="form-control" value="<?php echo htmlspecialchars($currentUser['username']); ?>" disabled>
                    <small style="color: var(--text-secondary);">Username cannot be changed</small>
                </div>

                <div class="form-group">
                    <label for="email">Email</label>
                    <input type="email" id="email" name="email" class="form-control" value="<?php echo htmlspecialchars($currentUser['email']); ?>" required>
                </div>

                <div class="form-group">
                    <label for="profile_picture">Profile Picture</label>
                    <input type="file" id="profile_picture" name="profile_picture" class="form-control" accept="image/*" onchange="previewImage(this, 'profilePreview')">
                    <img id="profilePreview" src="<?php echo htmlspecialchars($currentUser['profile_picture']); ?>" style="max-width: 150px; margin-top: 1rem; border-radius: 50%;">
                </div>

                <button type="submit" class="btn btn-primary" style="width: 100%;">Save Changes</button>
            </form>
        </div>
    </div>

    <!-- Create Quote Modal -->
    <div class="modal" id="createQuoteModal">
        <div class="modal-content">
            <div class="modal-header">
                <h2>Create New Quote</h2>
                <button class="close-modal" onclick="closeModal('createQuoteModal')">×</button>
            </div>
            <form id="createQuoteForm" onsubmit="handleCreateQuote(event)">
                <div class="form-group">
                    <label>Quote Text *</label>
                    <textarea name="quote_text" class="form-control" required placeholder="Share your thoughts..."></textarea>
                </div>
                
                <div class="form-group">
                    <label>Author</label>
                    <input type="text" name="author_name" class="form-control" placeholder="self (or author name)">
                </div>
                
                <div class="form-group">
                    <label>Category</label>
                    <select name="category" class="form-control">
                        <option value="">Select category</option>
                        <option value="Motivation">Motivation</option>
                        <option value="Love">Love</option>
                        <option value="Life">Life</option>
                        <option value="Happiness">Happiness</option>
                        <option value="Wisdom">Wisdom</option>
                        <option value="Friendship">Friendship</option>
                        <option value="Success">Success</option>
                    </select>
                </div>
                
                <div class="form-group">
                    <label>Mood</label>
                    <select name="mood" class="form-control">
                        <option value="">Select mood</option>
                        <option value="Happy">Happy</option>
                        <option value="Calm">Calm</option>
                        <option value="Inspired">Inspired</option>
                        <option value="Thoughtful">Thoughtful</option>
                        <option value="Sad">Sad</option>
                        <option value="Grateful">Grateful</option>
                    </select>
                </div>
                
                <div class="form-group">
                    <label>Add Image (optional)</label>
                    <input type="file" name="image" class="form-control" accept="image/*" onchange="previewImage(this, 'createImagePreview')">
                    <img id="createImagePreview" style="max-width: 100%; margin-top: 1rem; border-radius: 15px; display: none;">
                </div>
                
                <button type="submit" class="btn btn-primary" style="width: 100%;">Post Quote</button>
            </form>
        </div>
    </div>

    <script>
        window.currentUserId = <?php echo $currentUser['user_id']; ?>;
        
        // Load user's quotes on page load
        loadQuotes({ user_id: window.currentUserId });
        
        // Tab switching
        function switchTab(tabName) {
            // Update tab buttons
            document.querySelectorAll('.tab').forEach(tab => tab.classList.remove('active'));
            event.target.classList.add('active');
            
            // Show/hide content
            document.querySelectorAll('.tab-content').forEach(content => content.classList.add('hidden'));
            document.getElementById(tabName).classList.remove('hidden');
            
            // Load appropriate content
            if (tabName === 'my-quotes') {
                const container = document.getElementById('myQuotesContainer');
                container.innerHTML = '<div class="spinner"></div>';
                loadQuotes({ user_id: window.currentUserId });
            } else if (tabName === 'favorites') {
                loadFavorites();
            }
        }
        
        function loadQuotes(filters = {}) {
            const container = document.getElementById('myQuotesContainer');
            if (!container) return;
            
            container.innerHTML = '<div class="spinner"></div>';
            
            const params = new URLSearchParams(filters);
            fetch(`quotes.php?action=get_quotes&${params}`)
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        if (data.quotes.length === 0) {
                            container.innerHTML = '<p class="text-center">No quotes yet.</p>';
                        } else {
                            container.innerHTML = data.quotes.map(quote => createQuoteCard(quote)).join('');
                        }
                    }
                });
        }
        
        // Load favorites
        async function loadFavorites() {
            const container = document.getElementById('favoritesContainer');
            container.innerHTML = '<div class="spinner"></div>';
            
            try {
                const response = await fetch('quotes.php?action=get_favorites');
                const data = await response.json();
                
                if (data.success) {
                    if (data.quotes.length === 0) {
                        container.innerHTML = '<p class="text-center">No favorites yet. Start favoriting quotes!</p>';
                    } else {
                        container.innerHTML = data.quotes.map(quote => createQuoteCard(quote)).join('');
                    }
                }
            } catch (error) {
                console.error('Error loading favorites:', error);
            }
        }
        
        // Handle profile update
        async function handleUpdateProfile(e) {
            e.preventDefault();
            
            const form = e.target;
            const formData = new FormData(form);
            formData.append('action', 'update_profile');
            
            try {
                const response = await fetch('auth.php', {
                    method: 'POST',
                    body: formData
                });
                const data = await response.json();
                
                if (data.success) {
                    showAlert('Profile updated successfully!');
                    closeModal('editProfileModal');
                    setTimeout(() => location.reload(), 1000);
                } else {
                    showAlert(data.message, 'error');
                }
            } catch (error) {
                console.error('Error:', error);
                showAlert('Something went wrong', 'error');
            }
        }
        
        // Handle create quote
        async function handleCreateQuote(e) {
            e.preventDefault();
            
            const form = e.target;
            const formData = new FormData(form);
            formData.append('action', 'create_quote');
            
            try {
                const response = await fetch('quotes.php', {
                    method: 'POST',
                    body: formData
                });
                const data = await response.json();
                
                if (data.success) {
                    showAlert('Quote created successfully!');
                    closeModal('createQuoteModal');
                    form.reset();
                    document.getElementById('createImagePreview').style.display = 'none';
                    loadQuotes({ user_id: window.currentUserId });
                } else {
                    showAlert(data.message, 'error');
                }
            } catch (error) {
                console.error('Error:', error);
                showAlert('Something went wrong', 'error');
            }
        }
        
        // Logout function
        async function logout() {
            if (!confirm('Are you sure you want to logout?')) return;
            
            try {
                const formData = new FormData();
                formData.append('action', 'logout');
                
                const response = await fetch('auth.php', {
                    method: 'POST',
                    body: formData
                });
                const data = await response.json();
                
                if (data.success) {
                    window.location.href = 'index.php';
                }
            } catch (error) {
                console.error('Error:', error);
            }
        }
    </script>
    <script src="app.js"></script>
</body>
</html>