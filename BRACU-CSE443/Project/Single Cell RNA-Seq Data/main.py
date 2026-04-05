# main.py
import os
import scanpy as sc
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans, AgglomerativeClustering
from sklearn.mixture import GaussianMixture
from sklearn.metrics import silhouette_score
from scipy import sparse
import warnings

warnings.filterwarnings("ignore")

# LOCAL FILE
FILE_NAME = r"D:\\Project\\Single Cell RNA-Seq Data\\dataset\\V1_Human_Heart_filtered_feature_bc_matrix.h5"
N_CLUSTERS = 6   # change as needed
N_PCS = 20       # number of PCs to use for clustering

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
    # 1) load local file
    if not os.path.exists(FILE_NAME):
        print(f"File {FILE_NAME} not found! Place it in your project folder.")
        return

    print("Loading dataset...")
    adata = sc.read_10x_h5(FILE_NAME)
    print(adata)

    # 2) basic preprocess
    print("Preprocessing: normalize -> log1p -> HVG -> scale")
    sc.pp.normalize_total(adata, target_sum=1e4)
    sc.pp.log1p(adata)
    sc.pp.highly_variable_genes(adata, n_top_genes=2000, subset=True)
    sc.pp.scale(adata, max_value=10)

    # 3) PCA + neighbors + UMAP for visualization
    sc.tl.pca(adata, svd_solver='arpack')
    sc.pp.neighbors(adata, n_neighbors=10, use_rep='X_pca')
    sc.tl.umap(adata)

    # features for clustering
    X = adata.obsm['X_pca'][:, :N_PCS]
    X = np.asarray(X)

    # 4) K-Means
    print("Running KMeans...")
    kmeans_labels = KMeans(n_clusters=N_CLUSTERS, random_state=0).fit_predict(X)
    adata.obs['kmeans'] = kmeans_labels.astype(str)

    # 5) Gaussian Mixture Model
    print("Running Gaussian Mixture Model...")
    gmm = GaussianMixture(n_components=N_CLUSTERS, covariance_type='full', random_state=0)
    gmm_labels = gmm.fit_predict(X)
    adata.obs['gmm'] = gmm_labels.astype(str)

    # 6) Hierarchical Clustering
    print("Running Hierarchical Clustering...")
    hc = AgglomerativeClustering(n_clusters=N_CLUSTERS)
    hc_labels = hc.fit_predict(X)
    adata.obs['hierarchical'] = hc_labels.astype(str)

    # 7) compute silhouette scores
    print("Silhouette scores (higher is better):")
    for name, labels in [('kmeans', kmeans_labels), ('gmm', gmm_labels), ('hierarchical', hc_labels)]:
        try:
            score = silhouette_score(X, labels)
            print(f"  {name}: {score:.4f}")
        except Exception as e:
            print(f"  {name}: silhouette score error ->", e)

    # 8) UMAP plots
    print("Saving UMAP plots...")
    for col in ['kmeans', 'gmm', 'hierarchical']:
        sc.pl.umap(adata, color=col, show=False)
        safe_save_plot(f"umap_{col}.png")

    # 9) save cluster assignments
    out_csv = "cluster_assignments.csv"
    adata.obs[['kmeans', 'gmm', 'hierarchical']].to_csv(out_csv)
    print("Saved cluster assignments to", out_csv)

    out_h5ad = "human_heart_clustered.h5ad"
    adata.write(out_h5ad)
    print("Saved annotated AnnData to", out_h5ad)
    print("Done.")

if __name__ == "__main__":

    main()
