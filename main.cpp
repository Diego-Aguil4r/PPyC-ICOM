#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <omp.h> // OpenMP library

using namespace std;

const int WIDTH = 7680;
const int HEIGHT = 4320;
const int MAX_ITER = 256;

struct Color {
    unsigned char r, g, b;
};

// Task A: Generate Mandelbrot Set (Parallelized)
void generateMandelbrot(vector<Color>& image) {
    // Guided: Decreasing chunk sizes to reduce overhead
    #pragma omp parallel for schedule(guided)
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double c_re = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double c_im = (y - HEIGHT / 2.0) * 4.0 / WIDTH;
            double z_re = 0, z_im = 0;
            int iter = 0;

            while (z_re * z_re + z_im * z_im <= 4 && iter < MAX_ITER) {
                double z_re_new = z_re * z_re - z_im * z_im + c_re;
                z_im = 2 * z_re * z_im + c_im;
                z_re = z_re_new;
                iter++;
            }

            int index = y * WIDTH + x;
            if (iter == MAX_ITER) {
                image[index] = {0, 0, 0};
            } else {
                image[index] = {
                    static_cast<unsigned char>(iter % 256),
                    static_cast<unsigned char>((iter * 5) % 256),
                    static_cast<unsigned char>((iter * 10) % 256)
                };
            }
        }
    }
}

// Task B: Apply 5x5 Gaussian Blur (Parallelized)
void applyGaussianBlur(const vector<Color>& input, vector<Color>& output) {
    double kernel[5][5] = {
        { 1/256.0,  4/256.0,  6/256.0,  4/256.0, 1/256.0 },
        { 4/256.0, 16/256.0, 24/256.0, 16/256.0, 4/256.0 },
        { 6/256.0, 24/256.0, 36/256.0, 24/256.0, 6/256.0 },
        { 4/256.0, 16/256.0, 24/256.0, 16/256.0, 4/256.0 },
        { 1/256.0,  4/256.0,  6/256.0,  4/256.0, 1/256.0 }
    };

    #pragma omp parallel for
    for (int y = 2; y < HEIGHT - 2; ++y) {
        for (int x = 2; x < WIDTH - 2; ++x) {
            double r = 0, g = 0, b = 0;

            for (int ky = -2; ky <= 2; ++ky) {
                for (int kx = -2; kx <= 2; ++kx) {
                    double weight = kernel[ky + 2][kx + 2];
                    Color pixel = input[(y + ky) * WIDTH + (x + kx)];
                    r += pixel.r * weight;
                    g += pixel.g * weight;
                    b += pixel.b * weight;
                }
            }

            int index = y * WIDTH + x;
            output[index].r = static_cast<unsigned char>(r);
            output[index].g = static_cast<unsigned char>(g);
            output[index].b = static_cast<unsigned char>(b);
        }
    }
}

// ==========================================
// PUNTO 4: Histograma con exclusión mutua (Atomic)
// ==========================================
void calculateHistogramAtomic(const vector<Color>& image, int histogram[256]) {
    for(int i = 0; i < 256; i++) histogram[i] = 0;
    
    #pragma omp parallel for
    for (int i = 0; i < image.size(); ++i) {
        int val = image[i].r; // Usamos el canal rojo para el cálculo
        #pragma omp atomic
        histogram[val]++;
    }
}

// ==========================================
// PUNTO 4: Histograma con variables estrictamente locales
// ==========================================
void calculateHistogramEfficient(const vector<Color>& image, int histogram[256]) {
    for(int i = 0; i < 256; i++) histogram[i] = 0;
    
    #pragma omp parallel
    {
        int local_hist[256] = {0}; // Copia local para cada hilo
        
        #pragma omp for nowait
        for (int i = 0; i < image.size(); ++i) {
            local_hist[image[i].r]++;
        }
        
        // Se combinan los resultados locales en el arreglo global
        #pragma omp critical
        {
            for(int i = 0; i < 256; i++) {
                histogram[i] += local_hist[i];
            }
        }
    }
}

void savePPM(const string& filename, const vector<Color>& image) {
    ofstream file(filename, ios::binary);
    file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (const auto& pixel : image) {
        file << pixel.r << pixel.g << pixel.b;
    }
    file.close();
}

int main() {
    vector<Color> image(WIDTH * HEIGHT);
    vector<Color> blurred_image(WIDTH * HEIGHT);
    int histogram[256]; // Arreglo para almacenar los resultados del histograma

    cout << "Generating Mandelbrot 8K in parallel..." << endl;
    double start = omp_get_wtime();
    generateMandelbrot(image);
    cout << "Mandelbrot Time: " << omp_get_wtime() - start << "s\n" << endl;

    cout << "Applying Gaussian blur in parallel..." << endl;
    start = omp_get_wtime();
    applyGaussianBlur(image, blurred_image);
    cout << "Blur Time: " << omp_get_wtime() - start << "s\n" << endl;

    // Ejecución y medición del Punto 4
    cout << "Calculating Histogram (Atomic)..." << endl;
    start = omp_get_wtime();
    calculateHistogramAtomic(blurred_image, histogram);
    cout << "Histogram Atomic Time: " << omp_get_wtime() - start << "s\n" << endl;

    cout << "Calculating Histogram (Efficient/Local)..." << endl;
    start = omp_get_wtime();
    calculateHistogramEfficient(blurred_image, histogram);
    cout << "Histogram Local Time: " << omp_get_wtime() - start << "s\n" << endl;

    cout << "Saving image to disk..." << endl;
    savePPM("mandelbrot_blur.ppm", blurred_image);
    cout << "Process completed successfully." << endl;
    
    return 0;
}