# Software Requirements Specification (SRS)

**Course:** CSE470 – Software Engineering  
**Project Name:** LensLy – AI-Powered Eyewear E-Commerce Platform  

---

## Brief Idea
LensLy is a web-based e-commerce platform specialized in selling eyewear (frames, sunglasses, and prescription glasses). Customers can browse and purchase frames, select their preferred lens type, and enter their prescription power during checkout. The platform features an AI-powered face scanning tool that uses the customer's webcam to detect their face shape and skin tone, then suggests the most suitable frames from the current stock.

Stock, orders, offers, and consultation data are managed through Google Sheets as a secondary management layer, allowing non-technical admins to monitor everything from a spreadsheet. A full admin dashboard is also available for admins who prefer a GUI, supporting CSV-based stock uploads, offer management, and order tracking. Email notifications are handled through Google Apps Script. Customers who fill out a doctor consultation request form receive a 10% discount on their doctor visit if they own a LensLy frame.

---

## 20 Features (Sorted by Importance)

Here are the features logically ordered from core e-commerce functionality and key unique selling propositions (USPs) down to secondary management and nice-to-have features.

| Priority | Feature | Description |
| :--- | :--- | :--- |
| **1** | **Product Browsing & Filtering** | Customers can browse all available eyewear and filter by gender, color, and price range |
| **2** | **Product Search** | Customers can search for frames by name, brand, or type directly from the navbar in real time |
| **3** | **Product Detail Page** | Each product has a dedicated page showing images, description, stock status, and pricing |
| **4** | **Lens Type & Power Selector** | While ordering, customers can choose lens type (single vision, bifocal, photochromic) and enter their prescription power (SPH, CYL, AXIS) |
| **5** | **Add to Cart & Cart Management** | Customers can add frames to cart, update quantity, and remove items |
| **6** | **Checkout & Order Placement** | Customers fill in delivery details and place orders which are saved and synced to Google Sheets |
| **7** | **AI Face Scan & Frame Suggester** | Webcam scans the customer's face shape and skin tone and suggests the best matching frames from current stock |
| **8** | **Admin Dashboard Overview** | Admin sees a summary of total orders, revenue, active offers, low stock alerts, and consultation requests in one place |
| **9** | **Order Management & Status Update** | Admin can view all placed orders and update their status (Pending → Processing → Shipped → Delivered) |
| **10** | **Manual Stock Management** | Admin can individually edit product details, price, and stock count directly from the dashboard |
| **11** | **Google Sheets Sync** | All stock, orders, active offers, and consultation data automatically reflect in their respective Google Sheets tabs in real time |
| **12** | **Order Confirmation Email** | Google Apps Script automatically sends an order confirmation email to the customer upon successful order placement |
| **13** | **Doctor Consultation Request Form** | Customers can submit a form requesting a physical consultation with a doctor, including their issue and preferred time |
| **14** | **Consultation Management** | Admin can view all doctor consultation requests and mark them as contacted or resolved |
| **15** | **Consultation Discount Code Generation** | Customers who book a consultation and own a LensLy frame automatically receive a 10% discount code on their doctor visit via email |
| **16** | **Consultation Booking Confirmation Email** | Google Apps Script sends a confirmation email to the customer after a consultation request is submitted, including their discount code |
| **17** | **Dynamic Offer & Discount System** | Admin can create offers with a name, discount percentage, and time frame which automatically apply to all products sitewide during the active period |
| **18** | **Offer Management** | Admin can create, edit, activate, deactivate, and delete offers from the dashboard |
| **19** | **CSV Stock Upload** | Admin can upload a CSV file to bulk add or update product stock which syncs to Google Sheets |
| **20** | **Product Reviews** | Customers can give Star+Text review for each product and the product will show the average of total stars |

---

## Tech Stack

* **Frontend:** HTML, CSS, Vanilla JavaScript
* **Backend:** Express.js, Node.js serverless
* **Database:** Supabase (Postgres)
* **Secondary Data Layer:** Google Sheets API + Google Apps Script
* **AI:** Gemini API
* **Architecture:** MVC (Model-View-Controller)
