#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define PY_SSIZE_T_CLEAN
#include <Python.h>

double dist(double *vector1, double *vector2, int d);
int *tag(double **data, double **centroids, int n, int K, int d);
void vectors_average(double **data, double **centroids, int *tags_array, int n, int K, int d);
PyObject *kmeans(int K, int n, int d, double **data, int iter, double **centroids, double EPSILON)

#define INFINITY (__builtin_inff ())

double dist(double *vector1, double *vector2, int d) {
    double distance = 0.0;
    int i;
    for (i = 0; i<d; i++) {
        double diff = vector1[i]- vector2[i];
        distance+= diff*diff;
    }
    return sqrt(distance);
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

PyObject *kmeans(int K, int n, int d, double **data, int iter, double **centroids, double EPSILON) {
    int i, j, curr_iter, close_enough;
    double **prev_centroids;
    int *tags_array;

    prev_centroids= malloc(sizeof(double *) *K);
    for (i=0; i<K; i++) {
        prev_centroids[i]= malloc(sizeof(double ) *d);
        for (j=0; j<d; j++) {
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
    PyObject *centroids_list = PyList_New(K);
    for (i=0; i<K; i++) {
        PyObject *centroid = PyList_New(d);
        for (j=0; j<d; j++) {
            PyObject *coordinate = PyFloat_FromDouble(centroids[i][j]);
            PyList_SetItem(centroid, j, coordinate);
        }
        PyList_SetItem(centroids_list, i, centroid);
    }

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

    return centroids_list;
}

static PyObject *fit(PyObject *self, PyObject *args) {
    PyObject *data_points_obj;
    PyObject *centroids_obj;
    int K;
    int iter;
    double EPSILON;
    if (!PyArg_ParseTuple(args, "OOiid", &data_points_obj, &centroids_obj, &K, &iter, &EPSILON)) {
        return NULL;
    }

    int n = PyList_Size(data_points_obj);
    int d = PyList_Size(PyList_GetItem(data_points_obj, 0));
    double **data_points = (double **)malloc(sizeof(double *) * n);
    for (int i=0; i<n; i++) {
        data_points[i] = (double *)malloc(sizeof(double) * d);
        PyObject *data_point = PyList_GetItem(data_points_obj, i);
        for (int j=0; j<d; j++) {
            PyObject *coordinate_obj = PyList_GetItem(data_point, j);
            data_points[i][j] = PyFloat_AsDouble(coordinate_obj);
        }
    }

    double **centroids = (double **)malloc(sizeof(double *) * K);
    for (int i=1; i < K; i++) {
        centroids[i] = (double *)malloc(sizeof(double) * d);
        PyObject *centroid_obj = PyList_GetItem(centroids_obj, i);
        for (int j=0; j<d; j++) {
            PyObject *coordinate_obj = PyList_GetItem(centroid_obj, j)
            centroids[i][j] = PyFloat_AsDouble(coordinate_obj);
        }
    }
    PyObject *result = kmeans(K, n, d, data_points, iter, centroids, EPSILON);
    if (result == NULL) {
        printf("An Error Has Occurred");
        return NULL;
    }
    return Py_BuildValue("0", result);
}

static PyMethodDef kmeans_methods[] = {
    {"fit", fit, METH_VARARGS, PyDoc_STR("expected input: list of data points, k first clusters, num of clusters, iter, EPSILON")},
    {NULL, NULL, 0, NULL}
};

static struct PyMethodDef kmeans_module = {
    PyMethodDef_HEAD_INIT, "kmeanssp", "kmeans algorithm for clustering data points", -1, kmeans_methods
};

PyMODINIT_FUNC PyInit_kmeanssp(void) {
    PyObject *mod;
    mod = PyModule_Create(&kmeans_module);
    if (mod == false) {
        printf("An Error Has Occurred");
        return NULL;
    }
    return mod;
}
