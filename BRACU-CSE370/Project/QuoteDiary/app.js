// Quote Diary - Main JavaScript

// Theme Toggle
function initTheme() {
    const theme = localStorage.getItem('theme') || 'light';
    document.documentElement.setAttribute('data-theme', theme);
    updateThemeIcon();
}

function toggleTheme() {
    const currentTheme = document.documentElement.getAttribute('data-theme');
    const newTheme = currentTheme === 'light' ? 'dark' : 'light';
    document.documentElement.setAttribute('data-theme', newTheme);
    localStorage.setItem('theme', newTheme);
    updateThemeIcon();
}

function updateThemeIcon() {
    const themeBtn = document.querySelector('.theme-toggle');
    if (themeBtn) {
        themeBtn.addEventListener('click', toggleTheme);
    }
    if (!themeBtn) return;
    
    const theme = document.documentElement.getAttribute('data-theme');
    themeBtn.textContent = theme === 'light' ? 'Dark Mode' : 'Light Mode';
}

// Mobile Menu Toggle
function toggleMobileMenu() {
    const navLinks = document.querySelector('.nav-links');
    navLinks.classList.toggle('active');
}

// Show Alert
function showAlert(message, type = 'success') {
    const alertDiv = document.createElement('div');
    alertDiv.className = `alert alert-${type}`;
    alertDiv.textContent = message;
    
    const container = document.querySelector('.container');
    if (container) {
        container.insertBefore(alertDiv, container.firstChild);
        setTimeout(() => alertDiv.remove(), 3000);
    }
}

// Format Date
function formatDate(dateString) {
    const date = new Date(dateString);
    const now = new Date();
    const diff = now - date;
    const seconds = Math.floor(diff / 1000);
    const minutes = Math.floor(seconds / 60);
    const hours = Math.floor(minutes / 60);
    const days = Math.floor(hours / 24);
    
    if (days > 7) {
        return date.toLocaleDateString();
    } else if (days > 0) {
        return `${days} day${days > 1 ? 's' : ''} ago`;
    } else if (hours > 0) {
        return `${hours} hour${hours > 1 ? 's' : ''} ago`;
    } else if (minutes > 0) {
        return `${minutes} minute${minutes > 1 ? 's' : ''} ago`;
    } else {
        return 'Just now';
    }
}

// Modal Functions
function openModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.classList.add('active');
    }
}

function closeModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.classList.remove('active');
    }
}

// Create Quote Card
function createQuoteCard(quote) {
    const isOwner = window.currentUserId && quote.user_id == window.currentUserId;
    const isLiked = quote.user_liked > 0;
    const isFavorited = quote.user_favorited > 0;
    
    return `
        <div class="card" data-quote-id="${quote.quote_id}">
            <div class="card-header">
                <img src="${quote.profile_picture || 'media/assets/default-avatar.png'}" alt="${quote.username}" class="avatar">
                <div class="user-info">
                    <h4>${quote.username}</h4>
                    <small>${formatDate(quote.date_added)}</small>
                </div>
                ${isOwner ? `
                <div style="margin-left: auto;">
                    <button class="btn-icon" onclick="editQuote(${quote.quote_id})">Edit</button>
                    <button class="btn-icon" onclick="deleteQuote(${quote.quote_id})">Delete</button>
                </div>
                ` : ''}
            </div>
            
            ${quote.image_url ? `<img src="${quote.image_url}" alt="Quote image" class="quote-image">` : ''}
            
            <div class="quote-text">${quote.quote_text}</div>
            
            <div class="quote-meta">
                ${quote.author_name && quote.author_name !== 'self' ? `<span class="tag">By: ${quote.author_name}</span>` : ''}
                ${quote.category ? `<span class="tag">${quote.category}</span>` : ''}
                ${quote.mood ? `<span class="tag">${quote.mood}</span>` : ''}
            </div>
            
            <div class="card-actions">
                <button class="action-btn ${isLiked ? 'active' : ''}" onclick="toggleLike(${quote.quote_id}, this)">
                    ${isLiked ? '❤️' : '🤍'} <span>${quote.likes_count || 0}</span>
                </button>
                <button class="action-btn" onclick="toggleComments(${quote.quote_id})">
                    💬 <span>${quote.comments_count || 0}</span>
                </button>
                <button class="action-btn ${isFavorited ? 'active' : ''}" onclick="toggleFavorite(${quote.quote_id}, this)">
                    ${isFavorited ? '⭐' : '☆'}
                </button>
            </div>
            
            <div class="comments-section hidden" id="comments-${quote.quote_id}">
                <div class="comments-list"></div>
                ${window.currentUserId ? `
                <div class="form-group mt-1">
                    <input type="text" class="form-control" placeholder="Add a comment..." onkeypress="handleCommentKeypress(event, ${quote.quote_id})">
                </div>
                ` : ''}
            </div>
        </div>
    `;
}

// Load Quotes
async function loadQuotes(filters = {}) {
    const quotesContainer = document.getElementById('quotesContainer');
    if (!quotesContainer) return;
    
    quotesContainer.innerHTML = '<div class="spinner"></div>';
    
    try {
        const params = new URLSearchParams(filters);
        const response = await fetch(`quotes.php?action=get_quotes&${params}`);
        const data = await response.json();
        
        if (data.success) {
            if (data.quotes.length === 0) {
                quotesContainer.innerHTML = '<p class="text-center">No quotes found. Be the first to share!</p>';
            } else {
                quotesContainer.innerHTML = data.quotes.map(quote => createQuoteCard(quote)).join('');
            }
        } else {
            quotesContainer.innerHTML = '<p class="text-center">Failed to load quotes</p>';
        }
    } catch (error) {
        console.error('Error loading quotes:', error);
        quotesContainer.innerHTML = '<p class="text-center">Error loading quotes</p>';
    }
}

// Toggle Like
async function toggleLike(quoteId, btn) {
    if (!window.currentUserId) {
        showAlert('Please login to like quotes', 'error');
        return;
    }
    
    try {
        const formData = new FormData();
        formData.append('action', 'toggle_like');
        formData.append('quote_id', quoteId);
        
        const response = await fetch('quotes.php', {
            method: 'POST',
            body: formData
        });
        const data = await response.json();
        
        if (data.success) {
            const countSpan = btn.querySelector('span');
            const currentCount = parseInt(countSpan.textContent);
            
            if (data.liked) {
                btn.classList.add('active');
                btn.innerHTML = `❤️ <span>${currentCount + 1}</span>`;
            } else {
                btn.classList.remove('active');
                btn.innerHTML = `🤍 <span>${currentCount - 1}</span>`;
            }
        }
    } catch (error) {
        console.error('Error toggling like:', error);
    }
}

// Toggle Favorite
async function toggleFavorite(quoteId, btn) {
    if (!window.currentUserId) {
        showAlert('Please login to favorite quotes', 'error');
        return;
    }
    
    try {
        const formData = new FormData();
        formData.append('action', 'toggle_favorite');
        formData.append('quote_id', quoteId);
        
        const response = await fetch('quotes.php', {
            method: 'POST',
            body: formData
        });
        const data = await response.json();
        
        if (data.success) {
            if (data.favorited) {
                btn.classList.add('active');
                btn.textContent = '⭐';
            } else {
                btn.classList.remove('active');
                btn.textContent = '☆';
            }
        }
    } catch (error) {
        console.error('Error toggling favorite:', error);
    }
}

// Toggle Comments
async function toggleComments(quoteId) {
    const commentsSection = document.getElementById(`comments-${quoteId}`);
    if (!commentsSection) return;
    
    if (commentsSection.classList.contains('hidden')) {
        commentsSection.classList.remove('hidden');
        await loadComments(quoteId);
    } else {
        commentsSection.classList.add('hidden');
    }
}

// Load Comments
async function loadComments(quoteId) {
    const commentsList = document.querySelector(`#comments-${quoteId} .comments-list`);
    if (!commentsList) return;
    
    try {
        const response = await fetch(`quotes.php?action=get_comments&quote_id=${quoteId}`);
        const data = await response.json();
        
        if (data.success) {
            if (data.comments.length === 0) {
                commentsList.innerHTML = '<p style="text-align: center; color: var(--text-secondary);">No comments yet</p>';
            } else {
                commentsList.innerHTML = data.comments.map(comment => `
                    <div class="comment">
                        <div class="comment-header">
                            <img src="${comment.profile_picture || 'media/assets/default-avatar.png'}" class="comment-avatar">
                            <strong>${comment.username}</strong>
                            <small style="margin-left: auto;">${formatDate(comment.date_commented)}</small>
                        </div>
                        <p>${comment.comment_text}</p>
                    </div>
                `).join('');
            }
        }
    } catch (error) {
        console.error('Error loading comments:', error);
    }
}

// Handle Comment Keypress
async function handleCommentKeypress(event, quoteId) {
    if (event.key === 'Enter') {
        const input = event.target;
        const commentText = input.value.trim();
        
        if (!commentText) return;
        
        try {
            const formData = new FormData();
            formData.append('action', 'add_comment');
            formData.append('quote_id', quoteId);
            formData.append('comment_text', commentText);
            
            const response = await fetch('quotes.php', {
                method: 'POST',
                body: formData
            });
            const data = await response.json();
            
            if (data.success) {
                input.value = '';
                await loadComments(quoteId);
                
                // Update comment count
                const card = document.querySelector(`[data-quote-id="${quoteId}"]`);
                const commentBtn = card.querySelector('.action-btn:nth-child(2) span');
                if (commentBtn) {
                    commentBtn.textContent = parseInt(commentBtn.textContent) + 1;
                }
            }
        } catch (error) {
            console.error('Error adding comment:', error);
        }
    }
}

// Delete Quote
async function deleteQuote(quoteId) {
    if (!confirm('Are you sure you want to delete this quote?')) return;
    
    try {
        const formData = new FormData();
        formData.append('action', 'delete_quote');
        formData.append('quote_id', quoteId);
        
        const response = await fetch('quotes.php', {
            method: 'POST',
            body: formData
        });
        const data = await response.json();
        
        if (data.success) {
            showAlert('Quote deleted successfully');
            document.querySelector(`[data-quote-id="${quoteId}"]`).remove();
        } else {
            showAlert(data.message, 'error');
        }
    } catch (error) {
        console.error('Error deleting quote:', error);
    }
}

// Edit Quote
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
        const editIdInput = document.getElementById('editQuoteId');
        const quoteTextInput = document.getElementById('quoteText');
        const authorNameInput = document.getElementById('authorName');
        const categorySelect = document.getElementById('category');
        const moodSelect = document.getElementById('mood');
        const modalTitle = document.getElementById('modalTitle');
        const submitBtn = document.getElementById('submitBtn');
        const imagePreview = document.getElementById('createImagePreview');

        if (!editIdInput || !quoteTextInput || !modalTitle || !submitBtn) {
            console.error('Edit form elements not found');
            showAlert('Edit form not available', 'error');
            return;
        }

        editIdInput.value = quote.quote_id;
        quoteTextInput.value = quote.quote_text || '';
        if (authorNameInput) authorNameInput.value = quote.author_name || '';
        if (categorySelect) categorySelect.value = quote.category || '';
        if (moodSelect) moodSelect.value = quote.mood || '';

        // Update modal title and button text
        modalTitle.textContent = 'Edit Quote';
        submitBtn.textContent = 'Update Quote';

        // Show existing image if available
        if (imagePreview) {
            if (quote.image_url) {
                imagePreview.src = quote.image_url;
                imagePreview.style.display = 'block';
            } else {
                imagePreview.style.display = 'none';
            }
        }

        // Open the modal
        openModal('createQuoteModal');
    } catch (error) {
        console.error('Error loading quote for edit:', error);
        showAlert('Something went wrong', 'error');
    }
}

// Search & Filter
function initSearch() {
    const searchInput = document.getElementById('searchInput');
    const categoryFilter = document.getElementById('categoryFilter');
    
    if (searchInput) {
        searchInput.addEventListener('input', debounce(() => {
            loadQuotes({
                search: searchInput.value,
                category: categoryFilter?.value || ''
            });
        }, 500));
    }
    
    if (categoryFilter) {
        categoryFilter.addEventListener('change', () => {
            loadQuotes({
                search: searchInput?.value || '',
                category: categoryFilter.value
            });
        });
    }
}

// Debounce helper
function debounce(func, wait) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

// Image Preview
function previewImage(input, previewId) {
    if (input.files && input.files[0]) {
        const reader = new FileReader();
        reader.onload = function(e) {
            const preview = document.getElementById(previewId);
            if (preview) {
                preview.src = e.target.result;
                preview.style.display = 'block';
            }
        };
        reader.readAsDataURL(input.files[0]);
    }
}

// Initialize on page load
document.addEventListener('DOMContentLoaded', function() {
    initTheme();
    updateThemeIcon();
    
    // Attach theme toggle
    const themeBtn = document.querySelector('.theme-toggle');
    if (themeBtn) {
        themeBtn.addEventListener('click', toggleTheme);
    }
    
    // Attach mobile menu toggle
    const menuToggle = document.querySelector('.menu-toggle');
    if (menuToggle) {
        menuToggle.addEventListener('click', toggleMobileMenu);
    }
    
    // Initialize search
    initSearch();
    
    // Close modals on outside click
    document.querySelectorAll('.modal').forEach(modal => {
        modal.addEventListener('click', function(e) {
            if (e.target === modal) {
                modal.classList.remove('active');
            }
        });
    });
});
