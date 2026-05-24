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

    cout << "Generating Mandelbrot 8K in parallel..." << endl;
    double start = omp_get_wtime();
    generateMandelbrot(image);
    cout << "Mandelbrot Time: " << omp_get_wtime() - start << "s" << endl;

    cout << "Applying Gaussian blur in parallel..." << endl;
    start = omp_get_wtime();
    applyGaussianBlur(image, blurred_image);
    cout << "Blur Time: " << omp_get_wtime() - start << "s" << endl;

    savePPM("mandelbrot_blur.ppm", blurred_image);
    return 0;
}