# Data

**Source:** UCI Machine Learning Repository — Bank Marketing Dataset
**Link:** https://archive.ics.uci.edu/dataset/222/bank+marketing
**File used:** `bank-additional-full.csv`
**Size:** 41,188 rows, 21 columns (~5.8 MB — under 50MB, committed directly to `data/raw/`)
**Collected by:** Moro, S., Cortez, P., & Rita, P. (2014), from a Portuguese bank's telemarketing campaigns (2008–2013)
**Delimiter:** semicolon (`;`), not comma

## Files

- `raw/bank-additional-full.csv` — original, unmodified download
- `processed/` — cleaned dataset output from `notebooks/02_preprocessing.ipynb` (duration dropped, pdays sentinel handled)

## How to obtain independently

1. Visit https://archive.ics.uci.edu/dataset/222/bank+marketing
2. Download the dataset zip
3. Extract `bank-additional-full.csv` from the `bank-additional` folder
4. Place it in `data/raw/`

## Citation

Moro, S., Cortez, P., & Rita, P. (2014). A Data-Driven Approach to Predict the Success of Bank Telemarketing. Decision Support Systems, Elsevier, 62, 22-31.
