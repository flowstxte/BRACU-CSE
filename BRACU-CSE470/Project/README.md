# Glassé (formerly LensLy)

**Live Production Deployment:** [https://glasse.vercel.app](https://glasse.vercel.app/)  
**Specification Document:** [project_srs.md](project_srs.md)

---

## Project Overview

This project originated as **LensLy** for the **CSE470: Software Engineering** course at BRAC University. It was designed as an AI-powered e-commerce platform specifically tailored for eyewear—encompassing prescription glasses, designer frames, and sunglasses.

The project has since evolved and transitioned into an active production website rebranded as **Glassé**, serving as the official digital storefront for an eyewear brand. It introduces an expanded set of commercial capabilities, performance optimizations, and backend integrations.

The complete list of prioritized features, user flows, and technical requirements can be found in the [project_srs.md](project_srs.md).

---

## Key Capabilities and Architectural Highlights

### 1. Storefront and Customer Experience

- **Catalog Browsing and Real-Time Search:** Fast navigation across product categories with multi-attribute filtering (gender, frame style, color, price range) and dynamic instant search from the navigation bar.
- **Custom Lens and Prescription Configuration:** Interactive ordering workflow enabling customers to specify lens types (single vision, bifocal, photochromic) alongside optical parameters (Sphere, Cylinder, Axis).
- **AI-Powered Face Scanning and Recommendation:** Uses the customer's camera to analyze face shape and undertones, providing data-driven frame style recommendations matching in-stock inventory.
- **Consultation Booking and Partner Benefits:** Dedicated doctor consultation request flow providing verified customers with automated discount vouchers for clinical eye examinations.
- **Customer Reviews and Feedback:** Integrated star-rating and text review system calculating average ratings per item.

### 2. Back-Office and Administration

- **Centralized Dashboard:** High-level metrics tracking total orders, revenue analytics, pending doctor consultations, active promotions, and low-inventory warnings.
- **Order Lifecycle Management:** Step-by-step order tracking through status pipelines (Pending, Processing, Shipped, Delivered).
- **Dual-Layer Inventory System:** GUI-based product catalog management, bulk CSV catalog upload capabilities, and two-way Google Sheets synchronization for non-technical administration.
- **Dynamic Campaign Engine:** Site-wide discount scheduler with configurable timeframes and percentage markdowns.
- **Automated Communication:** Event-driven confirmation and notification pipelines for both orders and consultations powered by Google Apps Script.

---

## System Architecture and Technology Stack

- **Frontend:** Vanilla JavaScript, HTML5, Semantic CSS3
- **Backend Services:** Node.js, Express.js (Serverless architecture)
- **Database Layer:** Supabase (PostgreSQL)
- **Secondary Operations Layer:** Google Sheets API, Google Apps Script
- **Artificial Intelligence:** Google Gemini API (Visual face attribute recognition)
- **Architecture Pattern:** Model-View-Controller (MVC)

---

## Detailed Specifications

For complete documentation covering the 20 core features, priority matrix, operational data flows, and system requirements, refer to the [project_srs.md](project_srs.md) file located in this directory.

---

## Codebase Notice

For proprietary and security reasons, the private production codebase and commercial credentials are not published in this public academic repository.

To explore the live application in action, visit: [https://glasse.vercel.app](https://glasse.vercel.app/)
