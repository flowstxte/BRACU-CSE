<?php
// Database connection file
session_start();

// Database configuration
define('DB_HOST', 'localhost');
define('DB_USER', 'root');
define('DB_PASS', ''); // Default XAMPP password is empty
define('DB_NAME', 'quote_diary');

// Create connection
$conn = new mysqli(DB_HOST, DB_USER, DB_PASS, DB_NAME);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Set charset to UTF-8
$conn->set_charset("utf8mb4");

// Timezone
date_default_timezone_set('Asia/Dhaka');

// Helper function to sanitize input
function sanitize($data) {
    global $conn;
    // Strip magic_quotes slashes if PHP magic_quotes_gpc is on (old XAMPP)
    if (function_exists('get_magic_quotes_gpc') && get_magic_quotes_gpc()) {
        $data = stripslashes($data);
    }
    return $conn->real_escape_string(trim($data));
}

function sanitizeHTML($data) {
    return htmlspecialchars($data, ENT_QUOTES, 'UTF-8');
}

// Helper function to check if user is logged in
function isLoggedIn() {
    return isset($_SESSION['user_id']);
}

// Helper function to redirect
function redirect($url) {
    header("Location: $url");
    exit();
}

// Get current user info
function getCurrentUser() {
    global $conn;
    if (!isLoggedIn()) return null;
    
    $user_id = $_SESSION['user_id'];
    $stmt = $conn->prepare("SELECT user_id, username, email, profile_picture, date_joined FROM users WHERE user_id = ?");
    $stmt->bind_param("i", $user_id);
    $stmt->execute();
    $result = $stmt->get_result();
    return $result->fetch_assoc();
}
?>