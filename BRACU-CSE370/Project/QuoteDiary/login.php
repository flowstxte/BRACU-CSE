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
                <input type="password" id="password" name="password" class="form-control" required>
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
            
            const btn = form.querySelector('[type="submit"]');
            btn.disabled = true;
            btn.textContent = 'Please wait...';

            try {
                const response = await fetch('auth.php', {
                    method: 'POST',
                    body: formData
                });
                const data = await response.json();
                
                if (data.success) {
                    window.location.href = 'index.php';
                } else {
                    btn.disabled = false;
                    btn.textContent = 'Login';
                    showAlert(data.message, 'error');
                }
            } catch (error) {
                btn.disabled = false;
                btn.textContent = 'Login';
                console.error('Error:', error);
                showAlert('Something went wrong. Please try again.', 'error');
            }
        }

        function showAlert(message, type = 'success') {
            const alertDiv = document.createElement('div');
            alertDiv.className = `alert alert-${type}`;
            alertDiv.textContent = message;
            alertDiv.style.cssText = 'position:fixed;top:20px;left:50%;transform:translateX(-50%);z-index:9999;min-width:300px;text-align:center;box-shadow:0 4px 15px rgba(0,0,0,0.2);';
            document.body.appendChild(alertDiv);
            setTimeout(() => alertDiv.remove(), 4000);
        }
    </script>
    <script src="app.js"></script>
</body>
</html>
