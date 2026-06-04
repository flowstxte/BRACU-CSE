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
        <!-- Quote of the Day Section -->
        <div id="qotdSection" style="margin-bottom: 2rem;">
            <div style="text-align:center; padding: 1rem 0 0.5rem;">
                <span style="background: var(--accent-primary); color: white; padding: 0.3rem 1.2rem; border-radius: 20px; font-size: 0.85rem; font-weight: 700; letter-spacing: 1px; text-transform: uppercase;">Quote of the Day</span>
            </div>
            <div id="qotdCard" style="background: var(--gradient-1); border-radius: 30px; padding: 2.5rem 2rem; margin-top: 0.75rem; position: relative; overflow: hidden;">
                <!-- Decorative large quote mark -->
                <div style="position:absolute;top:-10px;left:10px;font-size:10rem;color:var(--accent-primary);opacity:0.08;line-height:1;pointer-events:none;">"</div>

                <div id="qotdContent" style="text-align:center;">
                    <div class="spinner"></div>
                </div>

                <?php if ($currentUser): ?>
                <div style="text-align:center; margin-top:1.5rem;">
                    <button class="btn btn-primary" onclick="openCreateModal()">+ Share Your Quote</button>
                </div>
                <?php else: ?>
                <div style="display:flex; gap:1rem; justify-content:center; margin-top:1.5rem; flex-wrap:wrap;">
                    <a href="signup.php" class="btn btn-primary">Get Started</a>
                    <a href="login.php" class="btn btn-secondary">Login</a>
                </div>
                <?php endif; ?>
            </div>
        </div>

        <!-- Search & Filter -->
        <div class="search-bar">
            <input type="text" id="searchInput" class="form-control search-input" placeholder="Search quotes or authors...">
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
            loadQuoteOfDay();
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

        async function loadQuoteOfDay() {
            const content = document.getElementById('qotdContent');
            if (!content) return;

            try {
                const response = await fetch('quotes.php?action=get_quote_of_day');
                const data = await response.json();

                if (data.success) {
                    const q = data.quote;
                    const isLiked = q.user_liked > 0;
                    const isFavorited = q.user_favorited > 0;

                    content.innerHTML = `
                        <p style="font-size:1.4rem;font-style:italic;line-height:1.8;color:var(--text-primary);margin-bottom:1rem;max-width:700px;margin-left:auto;margin-right:auto;">
                            ${q.quote_text}
                        </p>
                        <div style="display:flex;align-items:center;justify-content:center;gap:0.75rem;flex-wrap:wrap;margin-bottom:0.5rem;">
                            <img src="${q.profile_picture || 'media/assets/default-avatar.png'}" style="width:36px;height:36px;border-radius:50%;border:2px solid var(--accent-primary);object-fit:cover;">
                            <span style="font-weight:600;color:var(--text-primary);">${q.username}</span>
                            ${q.author_name && q.author_name.toLowerCase() !== 'self' && q.author_name.trim() !== '' ? `<span style="color:var(--text-secondary);">· ${q.author_name}</span>` : ''}
                            ${q.category ? `<span class="tag">${q.category}</span>` : ''}
                            ${q.mood ? `<span class="tag">${q.mood}</span>` : ''}
                        </div>
                        <div style="display:flex;justify-content:center;gap:1.5rem;margin-top:0.75rem;">
                            <button class="action-btn ${isLiked ? 'active' : ''}" onclick="toggleLike(${q.quote_id}, this)">
                                ${isLiked ? '❤️' : '🤍'} <span>${q.likes_count || 0}</span>
                            </button>
                            <button class="action-btn" onclick="toggleComments(${q.quote_id})">
                                💬 <span>${q.comments_count || 0}</span>
                            </button>
                            <button class="action-btn ${isFavorited ? 'active' : ''}" onclick="toggleFavorite(${q.quote_id}, this)">
                                ${isFavorited ? '⭐' : '☆'}
                            </button>
                        </div>
                        <div class="comments-section hidden" id="comments-${q.quote_id}">
                            <div class="comments-list"></div>
                            ${window.currentUserId ? `
                            <div class="form-group mt-1">
                                <input type="text" class="form-control" placeholder="Add a comment..." onkeypress="handleCommentKeypress(event, ${q.quote_id})">
                            </div>` : ''}
                        </div>
                    `;
                } else {
                    content.innerHTML = `<p style="color:var(--text-secondary);font-style:italic;">No quotes yet — be the first to share!</p>`;
                }
            } catch (err) {
                console.error('Error loading quote of day:', err);
                content.innerHTML = `<p style="color:var(--text-secondary);">Could not load quote of the day.</p>`;
            }
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