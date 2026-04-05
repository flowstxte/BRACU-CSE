# visium_local.py
import scanpy as sc
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans, AgglomerativeClustering
from sklearn.mixture import GaussianMixture
from sklearn.metrics import silhouette_score
from scipy import sparse
import warnings
import os

warnings.filterwarnings("ignore")

DATA_FOLDER = r"D:\\Project\\Spatial Transcriptomics Data_10X Visium\\dataset\\V1_Human_Lymph_Node"
N_CLUSTERS = 6   # adjust as needed
N_PCS = 20       # number of PCs to use

def get_dense_matrix(X):
    if sparse.issparse(X):
        return X.A
    return np.array(X)

def safe_save_plot(fig_name):
    plt.tight_layout()
    plt.savefig(fig_name, dpi=150)
    plt.close()
    print("Saved plot:", fig_name)

def main():
    print("Loading Visium dataset...")
    adata = sc.read_visium(DATA_FOLDER)
    print(adata)

    # Basic preprocessing
    print("Preprocessing: normalize -> log1p -> HVG -> scale")
    adata.layers['count'] = adata.X.toarray()
    sc.pp.filter_genes(adata, min_cells=50)
    sc.pp.filter_genes(adata, min_counts=10)
    sc.pp.normalize_total(adata, target_sum=1e4)
    sc.pp.log1p(adata)
    sc.pp.highly_variable_genes(adata, n_top_genes=2000, subset=True)
    sc.pp.scale(adata, max_value=10)

    # PCA
    sc.tl.pca(adata, svd_solver='arpack')
    X = adata.obsm['X_pca'][:, :N_PCS] if 'X_pca' in adata.obsm else get_dense_matrix(adata.X)
    X = np.asarray(X)

    # K-Means
    print("Running K-Means...")
    kmeans_labels = KMeans(n_clusters=N_CLUSTERS, random_state=0).fit_predict(X)
    adata.obs['kmeans'] = kmeans_labels.astype(str)

    # Gaussian Mixture
    print("Running Gaussian Mixture Model...")
    gmm = GaussianMixture(n_components=N_CLUSTERS, covariance_type='full', random_state=0)
    gmm_labels = gmm.fit_predict(X)
    adata.obs['gmm'] = gmm_labels.astype(str)

    # Hierarchical Clustering
    print("Running Hierarchical Clustering...")
    hier_labels = AgglomerativeClustering(n_clusters=N_CLUSTERS).fit_predict(X)
    adata.obs['hierarchical'] = hier_labels.astype(str)

    # Silhouette scores
    print("Silhouette scores (higher is better):")
    for name, labels in [('kmeans', kmeans_labels), ('gmm', gmm_labels), ('hierarchical', hier_labels)]:
        try:
            score = silhouette_score(X, labels)
            print(f"  {name}: {score:.4f}")
        except Exception as e:
            print(f"  {name}: silhouette score error ->", e)

    # UMAP visualization
    sc.pp.neighbors(adata, n_neighbors=10, use_rep='X_pca')
    sc.tl.umap(adata)
    for col in ['kmeans', 'gmm', 'hierarchical']:
        sc.pl.umap(adata, color=col, show=False)
        safe_save_plot(f"umap_{col}.png")

    # Save cluster assignments
    out_csv = "cluster_assignments.csv"
    adata.obs[['kmeans', 'gmm', 'hierarchical']].to_csv(out_csv)
    print("Saved cluster assignments to", out_csv)

    out_h5ad = "visium_clustered.h5ad"
    adata.write(out_h5ad)
    print("Saved annotated AnnData to", out_h5ad)
    print("Done.")

if __name__ == "__main__":
    main()

