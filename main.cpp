/*
 * Portfolio Project #2: Linear Regression (Dual Graphs on Single HTML Page)
 * Course: Quantitative Methods / Machine Language / PMP
 *
 * Description: Computes Linear Regression for POSITIVE and NEGATIVE correlations,
 * rendering side-by-side console graphs and opening a single browser page */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

struct RegressionResult {
    double slope;
    double intercept;
    double r_squared;
    string correlation_type;
};

double calculateMean(const vector<double>& data) {
    double sum = 0.0;
    for (double val : data) sum += val;
    return sum / data.size();
}

RegressionResult performLinearRegression(const vector<double>& x, const vector<double>& y) {
    size_t n = x.size();
    double x_mean = calculateMean(x);
    double y_mean = calculateMean(y);

    double numerator = 0.0, denominator = 0.0, ss_tot = 0.0;

    for (size_t i = 0; i < n; ++i) {
        numerator += (x[i] - x_mean) * (y[i] - y_mean);
        denominator += (x[i] - x_mean) * (x[i] - x_mean);
        ss_tot += (y[i] - y_mean) * (y[i] - y_mean);
    }

    double slope = numerator / denominator;
    double intercept = y_mean - (slope * x_mean);

    double ss_res = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double y_pred = slope * x[i] + intercept;
        ss_res += (y[i] - y_pred) * (y[i] - y_pred);
    }
    double r_squared = 1.0 - (ss_res / ss_tot);

    return {slope, intercept, r_squared, (slope > 0) ? "POSITIVE" : "NEGATIVE"};
}

// 1. Console ASCII Plotter
void renderDualConsoleGraph(const vector<double>& x_pos, const vector<double>& y_pos, const RegressionResult& res_pos,
                           const vector<double>& x_neg, const vector<double>& y_neg, const RegressionResult& res_neg) {
    const int WIDTH = 35;
    const int HEIGHT = 12;

    auto buildGrid = [&](const vector<double>& x, const vector<double>& y, double slope, double intercept, double& y_min_out, double& y_max_out) {
        double x_min = *min_element(x.begin(), x.end());
        double x_max = *max_element(x.begin(), x.end());
        y_min_out = *min_element(y.begin(), y.end()) - 2.0;
        y_max_out = *max_element(y.begin(), y.end()) + 2.0;

        vector<string> grid(HEIGHT, string(WIDTH, ' '));
        char line_char = (slope > 0) ? '/' : '\\';

        for (int col = 0; col < WIDTH; ++col) {
            double curr_x = x_min + col * (x_max - x_min) / (WIDTH - 1);
            double pred_y = slope * curr_x + intercept;
            int row = HEIGHT - 1 - static_cast<int>((pred_y - y_min_out) / (y_max_out - y_min_out) * (HEIGHT - 1));
            if (row >= 0 && row < HEIGHT) grid[row][col] = line_char;
        }

        for (size_t i = 0; i < x.size(); ++i) {
            int col = static_cast<int>((x[i] - x_min) / (x_max - x_min) * (WIDTH - 1));
            int row = HEIGHT - 1 - static_cast<int>((y[i] - y_min_out) / (y_max_out - y_min_out) * (HEIGHT - 1));
            if (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH) grid[row][col] = 'O';
        }
        return grid;
    };

    double y_min_p, y_max_p, y_min_n, y_max_n;
    vector<string> grid_pos = buildGrid(x_pos, y_pos, res_pos.slope, res_pos.intercept, y_min_p, y_max_p);
    vector<string> grid_neg = buildGrid(x_neg, y_neg, res_neg.slope, res_neg.intercept, y_min_n, y_max_n);

    cout << "\n   +--- POSITIVE CORRELATION ---+          +--- NEGATIVE CORRELATION ---+\n";
    for (int r = 0; r < HEIGHT; ++r) {
        double y_p = y_max_p - r * (y_max_p - y_min_p) / (HEIGHT - 1);
        double y_n = y_max_n - r * (y_max_n - y_min_n) / (HEIGHT - 1);

        cout << setw(5) << fixed << setprecision(1) << y_p << " | " << grid_pos[r] << " |    "
             << setw(5) << fixed << setprecision(1) << y_n << " | " << grid_neg[r] << " |\n";
    }
    cout << "      +---------------------------------+             +---------------------------------+\n";
    cout << "        Legend: [O] Point  [/] Line                      Legend: [O] Point  [\\] Line\n\n";
}

// 2. Multi-Graph HTML GUI Generator (Two Separate Charts on One Page)
void openMultiGraphBrowserGUI(const vector<double>& x_pos, const vector<double>& y_pos, const RegressionResult& res_pos,
                              const vector<double>& x_neg, const vector<double>& y_neg, const RegressionResult& res_neg) {
    ofstream htmlFile("dual_graphs_page.html");

    htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n";
    htmlFile << "<title>Linear Regression Visualizer</title>\n";
    htmlFile << "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n";
    htmlFile << "<style>\n";
    htmlFile << "  body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f0f2f5; margin: 30px; display: flex; flex-direction: column; align-items: center; }\n";
    htmlFile << "  .container { display: flex; flex-wrap: wrap; gap: 20px; justify-content: center; width: 100%; max-width: 1200px; }\n";
    htmlFile << "  .card { background: white; padding: 20px; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.08); flex: 1; min-width: 450px; max-width: 550px; }\n";
    htmlFile << "  h2 { text-align: center; color: #1e293b; margin-bottom: 5px; font-size: 20px; }\n";
    htmlFile << "  p { text-align: center; color: #64748b; font-size: 13px; margin-bottom: 15px; }\n";
    htmlFile << "</style>\n</head>\n<body>\n";

    htmlFile << "<h1 style=\"color: #0f172a; margin-bottom: 25px;\">Linear Regression Dashboard</h1>\n";
    htmlFile << "<div class=\"container\">\n";

    // Chart 1 Card: Positive
    htmlFile << "  <div class=\"card\">\n";
    htmlFile << "    <h2>Positive Correlation</h2>\n";
    htmlFile << "    <p><b>Equation:</b> Y = " << fixed << setprecision(4) << res_pos.slope << " * X + (" << res_pos.intercept << ") | <b>R²:</b> " << res_pos.r_squared << "</p>\n";
    htmlFile << "    <canvas id=\"posChart\"></canvas>\n";
    htmlFile << "  </div>\n";

    // Chart 2 Card: Negative
    htmlFile << "  <div class=\"card\">\n";
    htmlFile << "    <h2>Negative Correlation</h2>\n";
    htmlFile << "    <p><b>Equation:</b> Y = " << res_neg.slope << " * X + (" << res_neg.intercept << ") | <b>R²:</b> " << res_neg.r_squared << "</p>\n";
    htmlFile << "    <canvas id=\"negChart\"></canvas>\n";
    htmlFile << "  </div>\n";

    htmlFile << "</div>\n";

    // JavaScript to Render Both Charts Independently
    htmlFile << "<script>\n";

    auto writeArray = [&](const vector<double>& x, const vector<double>& y) {
        htmlFile << "[";
        for (size_t i = 0; i < x.size(); ++i) {
            htmlFile << "{x: " << x[i] << ", y: " << y[i] << "}" << (i == x.size() - 1 ? "" : ", ");
        }
        htmlFile << "]";
    };

    // Render Function Helper in JS
    htmlFile << "function createRegressionChart(canvasId, rawData, lineData, pointColor, lineColor) {\n";
    htmlFile << "  new Chart(document.getElementById(canvasId).getContext('2d'), {\n";
    htmlFile << "    type: 'scatter',\n";
    htmlFile << "    data: {\n";
    htmlFile << "      datasets: [\n";
    htmlFile << "        { label: 'Data Points', data: rawData, backgroundColor: pointColor, pointRadius: 6 },\n";
    htmlFile << "        { label: 'Fit Line', data: lineData, type: 'line', borderColor: lineColor, borderWidth: 3, fill: false, pointRadius: 0 }\n";
    htmlFile << "      ]\n";
    htmlFile << "    },\n";
    htmlFile << "    options: {\n";
    htmlFile << "      responsive: true,\n";
    htmlFile << "      scales: {\n";
    htmlFile << "        x: { title: { display: true, text: 'X Axis' } },\n";
    htmlFile << "        y: { title: { display: true, text: 'Y Axis' } }\n";
    htmlFile << "      }\n";
    htmlFile << "    }\n";
    htmlFile << "  });\n";
    htmlFile << "}\n\n";

    // Data 1
    htmlFile << "const posData = "; writeArray(x_pos, y_pos); htmlFile << ";\n";
    htmlFile << "const posLine = [{x: " << x_pos.front() << ", y: " << (res_pos.slope * x_pos.front() + res_pos.intercept)
             << "}, {x: " << x_pos.back() << ", y: " << (res_pos.slope * x_pos.back() + res_pos.intercept) << "}];\n";
    htmlFile << "createRegressionChart('posChart', posData, posLine, '#2563eb', '#1d4ed8');\n\n";

    // Data 2
    htmlFile << "const negData = "; writeArray(x_neg, y_neg); htmlFile << ";\n";
    htmlFile << "const negLine = [{x: " << x_neg.front() << ", y: " << (res_neg.slope * x_neg.front() + res_neg.intercept)
             << "}, {x: " << x_neg.back() << ", y: " << (res_neg.slope * x_neg.back() + res_neg.intercept) << "}];\n";
    htmlFile << "createRegressionChart('negChart', negData, negLine, '#dc2626', '#b91c1c');\n";

    htmlFile << "</script>\n</body>\n</html>\n";

    htmlFile.close();
    system("start dual_graphs_page.html");
}

int main() {
    vector<double> x_pos = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    vector<double> y_pos = {52.0, 58.0, 65.0, 70.0, 78.0, 82.0, 89.0, 95.0};

    vector<double> x_neg = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    vector<double> y_neg = {35.0, 30.0, 26.0, 22.0, 17.0, 14.0, 10.0, 6.0};

    RegressionResult res_pos = performLinearRegression(x_pos, y_pos);
    RegressionResult res_neg = performLinearRegression(x_neg, y_neg);

    cout << "\n======================================================================\n";
    cout << "             DUAL LINEAR REGRESSION ANALYSIS RESULTS                 \n";
    cout << "======================================================================\n";
    cout << " [1] POSITIVE CORRELATION:\n";
    cout << "     Line Eq  : Y = " << fixed << setprecision(4) << res_pos.slope << " * X + (" << res_pos.intercept << ")\n";
    cout << "     R-Square : " << res_pos.r_squared << "\n\n";

    cout << " [2] NEGATIVE CORRELATION:\n";
    cout << "     Line Eq  : Y = " << res_neg.slope << " * X + (" << res_neg.intercept << ")\n";
    cout << "     R-Square : " << res_neg.r_squared << "\n";
    cout << "======================================================================\n";

    // 1. Output ASCII Side-by-Side Plots in Console
    renderDualConsoleGraph(x_pos, y_pos, res_pos, x_neg, y_neg, res_neg);

    // 2. Open Page containing 2 GUI charts side-by-side
    cout << "Opening web page with 2 separate GUI charts...\n";
    openMultiGraphBrowserGUI(x_pos, y_pos, res_pos, x_neg, y_neg, res_neg);

    return 0;
}
