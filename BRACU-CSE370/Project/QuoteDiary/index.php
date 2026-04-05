<?php
require_once 'db_connect.php';

$currentUser = getCurrentUser();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Quote Diary</title>
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
                <?php if ($currentUser): ?>
                    <li><a href="profile.php">Profile</a></li>
                    <li><a href="#" onclick="openCreateModal()">New Quote</a></li>
                    <li><a href="#" onclick="logout()">Logout</a></li>
                <?php else: ?>
                    <li><a href="login.php">Login</a></li>
                    <li><a href="signup.php" class="btn btn-primary">Sign Up</a></li>
                <?php endif; ?>
                <li><button class="theme-toggle">Dark Mode</button></li>
            </ul>
        </div>
    </nav>

    <!-- Main Content -->
    <div class="container" style="margin-top: 2rem;">
        <?php if ($currentUser): ?>
            <!-- Quote of the Day Section -->
            <div style="background: var(--gradient-1); padding: 2rem; border-radius: 25px; margin-bottom: 2rem;">
                <h1 style="text-align:center;">Quote of the Day</h1>
                <p style="text-align:center; color: var(--text-secondary);">Most liked</p>
                <div id="quoteOfDayContainer" class="quotes-grid" style="margin-top: 1rem;">
                    <div class="spinner"></div>
                </div>
                <div style="text-align:center; margin-top:1rem;">
                    <button class="btn btn-primary" onclick="openCreateModal()">Create New Quote</button>
                </div>
            </div>
        <?php else: ?>
            <!-- Hero Section for non-logged users -->
            <div style="background: var(--gradient-1); padding: 3rem; border-radius: 30px; margin-bottom: 2rem; text-align: center;">
                <h1 style="font-size: 3rem; margin-bottom: 1rem;">Quote Diary</h1>
                <p style="font-size: 1.3rem; color: var(--text-secondary); margin-bottom: 2rem;">
                    Express yourself, inspire others, and build your personal collection of meaningful quotes
                </p>
                <div style="display: flex; gap: 1rem; justify-content: center; flex-wrap: wrap;">
                    <a href="signup.php" class="btn btn-primary">Get Started</a>
                    <a href="login.php" class="btn btn-secondary">Login</a>
                </div>
            </div>
        <?php endif; ?>

        <!-- Search & Filter -->
        <div class="search-bar">
            <input type="text" id="searchInput" class="form-control search-input" placeholder="Search quotes, authors, or usernames...">
            <select id="categoryFilter" class="filter-select">
                <option value="">All Categories</option>
                <option value="Motivation">Motivation</option>
                <option value="Love">Love</option>
                <option value="Life">Life</option>
                <option value="Happiness">Happiness</option>
                <option value="Wisdom">Wisdom</option>
                <option value="Friendship">Friendship</option>
                <option value="Success">Success</option>
            </select>
        </div>

        <!-- Quotes Grid -->
        <div class="quotes-grid" id="quotesContainer">
            <div class="spinner"></div>
        </div>
    </div>

    <!-- Create/Edit Quote Modal -->
    <?php if ($currentUser): ?>
    <div class="modal" id="createQuoteModal">
        <div class="modal-content">
            <div class="modal-header">
                <h2 id="modalTitle">Create New Quote</h2>
                <button class="close-modal" onclick="closeModal('createQuoteModal')">&times;</button>
            </div>
            <form id="createQuoteForm" onsubmit="handleCreateQuote(event)">
                <input type="hidden" name="quote_id" id="editQuoteId" value="">
                
                <div class="form-group">
                    <label>Quote Text *</label>
                    <textarea name="quote_text" id="quoteText" class="form-control" required placeholder="Share your thoughts..."></textarea>
                </div>
                
                <div class="form-group">
                    <label>Author</label>
                    <input type="text" name="author_name" id="authorName" class="form-control" placeholder="self (or author name)">
                </div>
                
                <div class="form-group">
                    <label>Category</label>
                    <select name="category" id="category" class="form-control">
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
                    <select name="mood" id="mood" class="form-control">
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
                
                <button type="submit" class="btn btn-primary" style="width: 100%;" id="submitBtn">Post Quote</button>
            </form>
        </div>
    </div>
    <?php endif; ?>

    <script>
        window.currentUserId = <?php echo $currentUser ? $currentUser['user_id'] : 'null'; ?>;
        
        // Load quotes on page load
        document.addEventListener('DOMContentLoaded', function() {
            loadQuoteOfTheDay();
            loadQuotesOnHome();
        });
        
        function loadQuotesOnHome() {
            const container = document.getElementById('quotesContainer');
            container.innerHTML = '<div class="spinner"></div>';
            
            const searchInput = document.getElementById('searchInput');
            const categoryFilter = document.getElementById('categoryFilter');
            
            const params = new URLSearchParams();
            if (searchInput && searchInput.value) {
                params.append('search', searchInput.value);
            }
            if (categoryFilter && categoryFilter.value) {
                params.append('category', categoryFilter.value);
            }
            
            fetch('quotes.php?action=get_quotes&' + params.toString())
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        if (data.quotes.length === 0) {
                            container.innerHTML = '<p class="text-center">No quotes found. Be the first to share!</p>';
                        } else {
                            container.innerHTML = data.quotes.map(quote => createQuoteCard(quote)).join('');
                        }
                    } else {
                        container.innerHTML = '<p class="text-center">Failed to load quotes</p>';
                    }
                })
                .catch(error => {
                    console.error('Error loading quotes:', error);
                    container.innerHTML = '<p class="text-center">Error loading quotes</p>';
                });
        }
        
        // Load Quote of the Day
        function loadQuoteOfTheDay() {
            const container = document.getElementById('quoteOfDayContainer');
            if (!container) return; // only present for logged-in users
            container.innerHTML = '<div class="spinner"></div>';
            
            fetch('quotes.php?action=get_quote_of_the_day')
                .then(response => response.json())
                .then(data => {
                    if (!data.success) {
                        container.innerHTML = '<p class="text-center">Failed to load Quote of the Day</p>';
                        return;
                    }
                    if (!data.quote) {
                        container.innerHTML = '<p class="text-center">No quotes yet. Create one!</p>';
                        return;
                    }
                    container.innerHTML = createQuoteCard(data.quote);
                })
                .catch(error => {
                    console.error('Error loading Quote of the Day:', error);
                    container.innerHTML = '<p class="text-center">Error loading Quote of the Day</p>';
                });
        }
        
        // Open modal for creating new quote
        function openCreateModal() {
            // Reset form
            document.getElementById('createQuoteForm').reset();
            document.getElementById('editQuoteId').value = '';
            document.getElementById('modalTitle').textContent = 'Create New Quote';
            document.getElementById('submitBtn').textContent = 'Post Quote';
            document.getElementById('createImagePreview').style.display = 'none';
            
            openModal('createQuoteModal');
        }
        
        // Edit quote function
        async function editQuote(quoteId) {
            try {
                const response = await fetch(`quotes.php?action=get_quote&quote_id=${quoteId}`);
                const data = await response.json();
                
                if (!data.success) {
                    showAlert('Failed to load quote', 'error');
                    return;
                }
                
                const quote = data.quote;
                
                // Fill form with quote data
                document.getElementById('editQuoteId').value = quote.quote_id;
                document.getElementById('quoteText').value = quote.quote_text;
                document.getElementById('authorName').value = quote.author_name || '';
                document.getElementById('category').value = quote.category || '';
                document.getElementById('mood').value = quote.mood || '';
                
                // Change modal title and button
                document.getElementById('modalTitle').textContent = 'Edit Quote';
                document.getElementById('submitBtn').textContent = 'Update Quote';
                
                // Show existing image if available
                if (quote.image_url) {
                    const preview = document.getElementById('createImagePreview');
                    preview.src = quote.image_url;
                    preview.style.display = 'block';
                }
                
                // Open modal
                openModal('createQuoteModal');
                
            } catch (error) {
                console.error('Error:', error);
                showAlert('Something went wrong', 'error');
            }
        }
        
        // Handle create/update quote
        async function handleCreateQuote(e) {
            e.preventDefault();
            
            const form = e.target;
            const formData = new FormData(form);
            const quoteId = document.getElementById('editQuoteId').value;
            
            // Set action based on whether we're editing or creating
            formData.append('action', quoteId ? 'update_quote' : 'create_quote');
            
            try {
                const response = await fetch('quotes.php', {
                    method: 'POST',
                    body: formData
                });
                const data = await response.json();
                
                if (data.success) {
                    showAlert(quoteId ? 'Quote updated successfully!' : 'Quote created successfully!');
                    closeModal('createQuoteModal');
                    form.reset();
                    document.getElementById('createImagePreview').style.display = 'none';
                    loadQuotesOnHome();
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
        
        // Search and filter
        const searchInput = document.getElementById('searchInput');
        const categoryFilter = document.getElementById('categoryFilter');
        
        if (searchInput) {
            let searchTimeout;
            searchInput.addEventListener('input', function() {
                clearTimeout(searchTimeout);
                searchTimeout = setTimeout(() => {
                    loadQuotesOnHome();
                }, 500);
            });
        }
        
        if (categoryFilter) {
            categoryFilter.addEventListener('change', function() {
                loadQuotesOnHome();
            });
        }
    </script>
    <script src="app.js"></script>
</body>
</html>
