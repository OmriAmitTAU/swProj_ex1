#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define E 0.001
#define MAX_DIMENSION_SIZE 100

int is_natural(double num) {
    return num > 0 && fmod(num, 1) == 0;
}

void assert_input(int k, int n, int d, int max_iter) {
    if (!(n > 1 && is_natural(n))) {
        printf("Invalid number of points!\n");
        exit(1);
    }
    if (!(1 < k && k < n && is_natural(k))) {
        printf("Invalid number of clusters!\n");
        exit(1);
    }
    if (!is_natural(d)) {
        printf("Invalid dimension of point!\n");
        exit(1);
    }
    if (!(1 < max_iter && max_iter < 1000 && is_natural(max_iter))) {
        printf("Invalid maximum iteration!\n");
        exit(1);
    }
}

void init_vector_list(double **vectors, char *input_data, int n, int d) {
    int i = 0;
    char *line_ptr = input_data;
    
    while (*line_ptr && i < n) {
        vectors[i] = malloc(d * sizeof(double));

        for (int j = 0; j < d; j++) {
            char *end_ptr;
            vectors[i][j] = strtod(line_ptr, &end_ptr);
            line_ptr = end_ptr;

            if (*line_ptr == ',') {
                line_ptr++; // Move past the comma
            } else {
                break; // End of line or error
            }
        }

        // Move to the next line
        while (*line_ptr && *line_ptr != '\n') {
            line_ptr++;
        }
        if (*line_ptr == '\n') {
            line_ptr++; // Move past the newline
        }

        i++;
    }
}

double distance(double *a, double *b, int d) {
    double dist = 0;
    for (int i = 0; i < d; i++) {
        dist += pow(a[i] - b[i], 2);
    }
    return sqrt(dist);
}

int find_closest_centroid(double *curr, double **k_centroids, int k, int d) {
    double closest_d = INFINITY;
    int closest_centroid = -1;

    for (int i = 0; i < k; i++) {
        double d = distance(curr, k_centroids[i], d);
        if (d < closest_d) {
            closest_d = d;
            closest_centroid = i;
        }
    }
    return closest_centroid;
}

void calculate_updated_centroid(double centroid_vectors[][MAX_DIMENSION_SIZE], double *updated_centroid, int num_vectors, int d) {
    for (int i = 0; i < d; i++) {
        double sum = 0;
        for (int j = 0; j < num_vectors; j++) {
            sum += centroid_vectors[j][i];
        }
        updated_centroid[i] = sum / num_vectors;
    }
}

void assign_to_closest_centroid(double **datapoints, int n, double **k_centroids, int *vectors_mapping, int k) {
    for (int i = 0; i < n; i++) {
        int closest_centroid = find_closest_centroid(datapoints[i], k_centroids, k, 2);
        vectors_mapping[i] = closest_centroid;
    }
}

void k_means(int k, int n, int d, char *input_data, int max_iter) {
    assert_input(k, n, d, max_iter);
    double *datapoints[n];
    int vectors_mapping[n];
    int i, j;

    init_vector_list(datapoints, input_data, n, d);
    double *k_centroids[k];

    for (i = 0; i < k; i++) {
        k_centroids[i] = datapoints[i];
    }

    int iter = 0;
    double delta_miu = INFINITY;

    while (delta_miu >= E && iter < max_iter) {
        assign_to_closest_centroid(datapoints, n, k_centroids, vectors_mapping, k);

        double *new_k_centroids[k];
        for (i = 0; i < k; i++) {
            int num_vectors = 0;
            for (j = 0; j < n; j++) {
                if (vectors_mapping[j] == i) {
                    num_vectors++;
                }
            }

            double centroid_vectors[num_vectors][d];
            int idx = 0;
            for (j = 0; j < n; j++) {
                if (vectors_mapping[j] == i) {
                    for (int l = 0; l < d; l++) {
                        centroid_vectors[idx][l] = datapoints[j][l];
                    }
                    idx++;
                }
            }

            new_k_centroids[i] = malloc(d * sizeof(double));
            calculate_updated_centroid(centroid_vectors, new_k_centroids[i], num_vectors, d);
        }

        delta_miu = 0;
        for (i = 0; i < k; i++) {
            double d = distance(k_centroids[i], new_k_centroids[i], d);
            if (d > delta_miu) {
                delta_miu = d;
            }
            k_centroids[i] = new_k_centroids[i];
        }

        iter++;
    }

    for (i = 0; i < k; i++) {
        printf("%.4f,%.4f\n", k_centroids[i][0], k_centroids[i][1]);
    }
}

int main() {
    int k, n, d, max_iter;
    scanf("%d %d %d %d\n", &k, &n, &d, &max_iter);
    char input_data[10000];
    fgets(input_data, sizeof(input_data), stdin);
    k_means(k, n, d, input_data, max_iter);
    return 0;
}
