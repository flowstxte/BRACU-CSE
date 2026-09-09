# CSE437 Project: Bank Term Deposit Subscription Prediction

**Group 2** — Ullas Sarker Tirtha (24141252), Sahana Parvin Nupur (24141251)

## Problem

Predicting whether a client will subscribe to a term deposit based on demographic, financial, and campaign-contact data, so a bank can prioritize telemarketing outreach more efficiently.

## Dataset

UCI Bank Marketing Dataset (`bank-additional-full.csv`), 41,188 records, sourced from https://archive.ics.uci.edu/dataset/222/bank+marketing. See `data/README.md` for details.

## Three Questions

1. Which attributes show the strongest statistical association with subscription?
2. Which feature engineering/selection approach best reduces dimensionality while preserving predictive signal?
3. How do Logistic Regression and AdaBoost differ in performance and error patterns on this imbalanced dataset?

## Models

Logistic Regression and AdaBoost, both trained with a scikit-learn preprocessing pipeline (one-hot encoding + scaling), tuned via grid search with stratified 5-fold cross-validation.

## How to Run

1. Clone this repository.
2. Install dependencies: `pip install -r requirements.txt`
3. Place `bank-additional-full.csv` in `data/raw/` (already committed — see `data/README.md`).
4. Run notebooks in order from `notebooks/`:
   - `01_data_audit_and_eda.ipynb`
   - `02_preprocessing.ipynb`
   - `03_feature_engineering.ipynb`
   - `04_modeling_and_tuning.ipynb`
   - `05_evaluation_and_error_analysis.ipynb`
5. Each notebook uses relative paths only and runs top to bottom on a fresh kernel.

## Report

See `report/report.pdf` (or `report/report.md`).

## Figures

All figures used in the report are saved in `figures/`.
