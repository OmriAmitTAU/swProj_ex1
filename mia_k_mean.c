#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define DEFAULT_ITERATION 200
#define MAX_ITERATION 1000

int is_natural(int num);
int check_input(int k, int n, int d, int max_iter);
double** read_data_from_file(int n, int d);
double dist(double *vector1, double *vector2, int d);
int *tag(double **data, double **centroids, int n, int K, int d);
void vectors_average(double **data, double **centroids, int *tags_array, int n, int K, int d);
void print_centroids(int K, int d, double **centroids);
void kmeans(int K, int n, int d, double **data, int iter);
int main(int argc, char *argv[]);

int is_natural(int num) {
    return num > 0;
}

int check_input(int k, int n, int d, int max_iter) {
    if (!(n > 1 && is_natural(n))) {
        printf("Invalid number of points!\n");
        return 0;
    }
    if (!(1 < k && k < n && is_natural(k))) {
        printf("Invalid number of clusters!\n");
        return 0;
    }
    if (!is_natural(d)) {
        printf("Invalid dimension of point!\n");
        return 0;
    }
    if (!(1 < max_iter && max_iter < 1000 && is_natural(max_iter))) {
        printf("Invalid maximum iteration!\n");
        return 0;
    }
    return 1;
}

double** read_data_from_file(int n, int d) {
    double** data = (double**)malloc(n * sizeof(double*));
    if (data == NULL) {
        printf("An Error Has Occurred!\n");
        return NULL;
    }   
    for (int i = 0; i < n; i++) {
        data[i] = (double*)malloc(d * sizeof(double));
        if (data[i] == NULL) {
            printf("An Error Has Occurred!\n");
            return NULL;
        }
        for (int j = 0; j < d; j++) {
            if (j < d - 1) {
                if (scanf("%lf,", &data[i][j]) != 1) {
                    printf("An Error Has Occurred!\n");
                    return NULL;
                }
            } else {
                if (scanf("%lf\n", &data[i][j]) != 1) {
                    printf("An Error Has Occurred!\n");
                    exit(1);
                }
            }
        }
    }
    return data;
}

double dist(double *vector1, double *vector2, int d) {
    double distance= 0;
    int i;
    for (i=0; i<d; i++) {
        double diff = vector1[i]- vector2[i];
        distance+= diff*diff;
    }
    double result = sqrt(distance);
    return result;
}

int *tag(double **data, double **centroids, int n, int K, int d) {
    int *tags_array;
    int i, j, tag;
    tags_array= malloc(sizeof(int)*n);
    for (i=0; i<n; i++) {
        double min_distance= INFINITY;
        for (j=0; j<K; j++) {
            double distance = dist(data[i], centroids[j], d);
            if (distance< min_distance) {
                min_distance= distance;
                tag= j;
            }
        }
        tags_array[i]= tag;
    }
    return tags_array;
}

void vectors_average(double **data, double **centroids, int *tags_array, int n, int K, int d) {
    int *clusters_size= (int *)malloc(sizeof(double)*K);
    int i,j;
    for (i=0; i<K; i++) {
        clusters_size[i]= 0;
        for (j=0; j<d; j++) {
            centroids[i][j]= 0;
        }
    }
    for (i=0; i<n; i++) {
        int cluster= tags_array[i];
        clusters_size[cluster]++;
        for (j=0; j<d; j++) {
            centroids[cluster][j]+= data[i][j];
        }
    }
    for (i=0; i<K; i++) {
        if (clusters_size[i] != 0) {
            int amount_vectors= clusters_size[i];
            for (j=0; j<d; j++) {
                double sum_vectors= centroids[i][j];
                centroids[i][j]=  (double) sum_vectors / amount_vectors;
            }
        }
    }
    free(clusters_size);
}

void print_centroids(int K, int d, double **centroids)
{
    for (int i = 0; i < K; i++)
    {
        for (int j = 0; j < d; j++)
        {
            printf("%.4f", centroids[i][j]);
            if (j < (d - 1))
            {
                printf(",");
            }
        }
        printf("\n");
    }
}

void kmeans(int K, int n, int d, double **data, int iter) {
    int i, j, curr_iter, close_enough;
    double EPSILON= 0.001;
    double **centroids;
    double **prev_centroids;
    int *tags_array;

    centroids= malloc(sizeof(double *) *K);
    prev_centroids= malloc(sizeof(double *) *K);
    for (i=0; i<K; i++) {
        centroids[i]= malloc(sizeof(double ) *d);
        prev_centroids[i]= malloc(sizeof(double ) *d);
        for (j=0; j<d; j++) {
            centroids[i][j]= data[i][j];
            prev_centroids[i][j]= centroids[i][j];
        }
    }

    tags_array= NULL;
    for (curr_iter=0; curr_iter<iter; curr_iter++) {
        tags_array= tag(data, centroids, n, K, d);
        vectors_average(data,centroids, tags_array, n, K, d);

        close_enough= 1;
        for (i=0; i<K; i++) {
            double distance= dist(centroids[i], prev_centroids[i], d);
            if (distance > EPSILON) {
                close_enough= 0;
                break;
            }
        }
        if (close_enough==1) {
            break;
        }
        
        free(tags_array);
        
        for (i=0; i<K; i++) {
            for(j=0; j<d; j++) {
                prev_centroids[i][j]= centroids[i][j];
            }
        }
    }
    print_centroids(K, d, centroids);

    for (i=0; i<K; i++) {
        free(prev_centroids[i]);
        free(centroids[i]);
    }
    free(prev_centroids);
    free(centroids);
    for (i=0; i<n; i++) {
        free(data[i]);
    }
    free(data);
}

int main(int argc, char *argv[])
{
    if (argc < 4 || argc > 6) {
        printf("An Error Has Occurred\n");
        return 1;
    }
    int k = atoi(argv[1]);
    int n = atoi(argv[2]);
    int d = atoi(argv[3]);
    int max_iter = argc == 6 ? atoi(argv[4]) : DEFAULT_ITERATION;

    if (!check_input(k, n, d, max_iter)) {
        return 1;
    }

    double** data = read_data_from_file(n, d);
    if (data == NULL) {
        printf("An Error Has Occurred!\n");
        return 1;
    }
    kmeans(k, n, d, data, max_iter);
    
    return 0;
}