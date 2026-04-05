<?php
require_once 'db_connect.php';

// Redirect if already logged in
if (isLoggedIn()) {
    redirect('index.php');
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login - Quote Diary</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <!-- Navigation -->
    <nav class="navbar">
        <div class="nav-content">
            <a href="index.php" class="logo">Quote Diary</a>
            
            <ul class="nav-links">
                <li><a href="index.php">Home</a></li>
                <li><a href="signup.php">Sign Up</a></li>
                <li><button class="theme-toggle">Dark Mode</button></li>
            </ul>
        </div>
    </nav>

    <!-- Login Form -->
    <div class="form-container">
        <div style="text-align: center; margin-bottom: 2rem;">
            <h1>Welcome Back!</h1>
            <p style="color: var(--text-secondary);">Login to continue sharing your thoughts</p>
        </div>

        <form id="loginForm" onsubmit="handleLogin(event)">
            <div class="form-group">
                <label for="username">Username or Email</label>
                <input type="text" id="username" name="username" class="form-control" required>
            </div>

            <div class="form-group">
                <label for="password">Password</label>
                <div class="input-with-action">
                    <input type="password" id="password" name="password" class="form-control" required>
                    <button type="button" class="toggle-password" onclick="togglePassword('password', this)">Show</button>
                </div>
            </div>

            <button type="submit" class="btn btn-primary" style="width: 100%;">Login</button>
        </form>

        <div style="text-align: center; margin-top: 1.5rem;">
            <p style="color: var(--text-secondary);">
                Don't have an account? 
                <a href="signup.php" style="color: var(--accent-primary); font-weight: 600;">Sign Up</a>
            </p>
        </div>
    </div>

    <script>
        async function handleLogin(e) {
            e.preventDefault();
            
            const form = e.target;
            const formData = new FormData(form);
            formData.append('action', 'login');
            
            try {
                const response = await fetch('auth.php', {
                    method: 'POST',
                    body: formData
                });
                const data = await response.json();
                
                if (data.success) {
                    window.location.href = 'index.php';
                } else {
                    showAlert(data.message, 'error');
                }
            } catch (error) {
                console.error('Error:', error);
                showAlert('Something went wrong. Please try again.', 'error');
            }
        }

        function togglePassword(id, btn) {
            const input = document.getElementById(id);
            if (!input) return;
            const isHidden = input.type === 'password';
            input.type = isHidden ? 'text' : 'password';
            btn.textContent = isHidden ? 'Hide' : 'Show';
        }

        function showAlert(message, type = 'success') {
            const alertDiv = document.createElement('div');
            alertDiv.className = `alert alert-${type}`;
            alertDiv.textContent = message;
            
            const form = document.getElementById('loginForm');
            form.parentElement.insertBefore(alertDiv, form);
            
            setTimeout(() => alertDiv.remove(), 4000);
        }
    </script>
    <script src="app.js"></script>
</body>
</html>