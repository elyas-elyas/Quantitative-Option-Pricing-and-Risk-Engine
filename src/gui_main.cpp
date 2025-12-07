#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <omp.h>

// Finance Headers
#include "BlackScholes.h"
#include "EuropeanOption.h"
#include "HestonMC.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    #if defined(__APPLE__)
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #else
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #endif

    GLFWwindow* window = glfwCreateWindow(1280, 850, "C++ Quantitative Trading Dashboard", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable V-Sync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // --- MARKET PARAMETERS ---
    float spot = 100.0f;
    float strike = 100.0f;
    float rate = 0.05f;
    float volatility = 0.20f;
    float maturity = 1.0f;
    int optionType = 0; 

    // --- HESTON PARAMETERS [NEW] ---
    // v0: Initial Variance
    // kappa: Mean reversion speed
    // theta: Long run variance
    // xi: Volatility of Volatility
    // rho: Correlation
    float h_kappa = 2.0f;
    float h_theta = 0.04f; // Equivalent to sqrt(0.04) = 20% long run vol
    float h_xi = 0.1f;
    float h_rho = -0.7f;   // Standard negative correlation in equity markets

    // Graph Buffers
    const int resolution = 100;
    std::vector<float> x_data(resolution);
    std::vector<float> y_bs(resolution);
    std::vector<float> y_heston(resolution); // Store Heston curve data

    // Heatmap Buffers
    const int hm_res = 80; // Reduced resolution for real-time performance
    std::vector<float> heatmap_values(hm_res * hm_res);
    float hm_spot_min = 50.0f, hm_spot_max = 150.0f;
    float hm_vol_min = 0.05f, hm_vol_max = 1.0f;

    // Heston Instance (Lower sim count for GUI responsiveness)
    HestonPricer hestonPricer(5000, 50); 

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

        // --- LEFT COLUMN: CONTROLS ---
        ImGui::BeginChild("Controls", ImVec2(320, 0), true);
        
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1), "1. STANDARD PARAMETERS");
        ImGui::Separator();
        ImGui::SliderFloat("Spot", &spot, 50.0f, 150.0f);
        ImGui::SliderFloat("Strike", &strike, 50.0f, 150.0f);
        ImGui::SliderFloat("Rate", &rate, 0.0f, 0.20f);
        ImGui::SliderFloat("BS Vol", &volatility, 0.01f, 1.0f);
        ImGui::SliderFloat("Maturity", &maturity, 0.1f, 5.0f);
        ImGui::RadioButton("Call", &optionType, 0); ImGui::SameLine();
        ImGui::RadioButton("Put", &optionType, 1);

        ImGui::Spacing(); ImGui::Spacing();

        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1), "2. HESTON PARAMETERS");
        ImGui::Separator();
        ImGui::SliderFloat("Kappa (Speed)", &h_kappa, 0.1f, 10.0f);
        ImGui::SliderFloat("Theta (L.T. Var)", &h_theta, 0.01f, 0.5f);
        ImGui::SliderFloat("Xi (Vol of Vol)", &h_xi, 0.01f, 1.0f);
        ImGui::SliderFloat("Rho (Corr)", &h_rho, -0.99f, 0.99f);
        
        ImGui::Spacing(); ImGui::Separator();

        // --- LIVE PRICING ---
        OptionType type = (optionType == 0) ? OptionType::CALL : OptionType::PUT;
        EuropeanOption opt(strike, maturity, type);
        
        // 1. Black-Scholes Price
        BlackScholes bs(spot, strike, rate, volatility, maturity, type);
        double priceBS = bs.price();

        // 2. Heston Price (Current Spot)
        // Note: v0 = volatility^2 to match BS input
        double v0 = volatility * volatility;
        double priceHeston = hestonPricer.price(opt, spot, rate, v0, h_kappa, h_theta, h_xi, h_rho);

        ImGui::TextColored(ImVec4(0, 1, 0, 1), "PRICING RESULTS");
        ImGui::Text("BS Price:      %.4f $", priceBS);
        ImGui::Text("Heston Price:  %.4f $", priceHeston);
        
        double diff = priceHeston - priceBS;
        ImGui::TextColored(diff > 0 ? ImVec4(1,0.3f,0.3f,1) : ImVec4(0.3f,0.3f,1,1), 
                          "Diff (Model Risk): %.4f $", diff);

        ImGui::EndChild();

        // --- RIGHT COLUMN: VISUALIZATION ---
        ImGui::SameLine();
        ImGui::BeginChild("Graphs", ImVec2(0, 0), true);
        
        if (ImGui::BeginTabBar("Tabs")) {
            
            // TAB 1: MODEL COMPARISON
            if (ImGui::BeginTabItem("Model Comparison")) {
                
                // Parallel calculation of the full curve
                #pragma omp parallel for
                for (int i = 0; i < resolution; ++i) {
                    float s = 50.0f + i * (100.0f / resolution); 
                    x_data[i] = s;
                    
                    // BS
                    BlackScholes tBS(s, strike, rate, volatility, maturity, type);
                    y_bs[i] = (float)tBS.price();

                    // Heston (Using local thread-safe instance)
                    HestonPricer localHeston(2000, 30); // Reduced precision for full graph speed
                    y_heston[i] = (float)localHeston.price(opt, s, rate, v0, h_kappa, h_theta, h_xi, h_rho);
                }

                if (ImPlot::BeginPlot("Black-Scholes vs Heston", ImVec2(-1, -1))) {
                    ImPlot::SetupAxes("Spot Price", "Option Value");
                    ImPlot::PlotLine("Black-Scholes", x_data.data(), y_bs.data(), resolution);
                    ImPlot::SetNextLineStyle(ImVec4(1, 0.5f, 0, 1)); // Orange for Heston
                    ImPlot::PlotLine("Heston Model", x_data.data(), y_heston.data(), resolution);
                    
                    ImPlot::TagX(spot, ImVec4(1,1,1,0.5f), "Spot");
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }

            // TAB 2: HEATMAP (Keep BS Heatmap for speed/visual clarity)
            if (ImGui::BeginTabItem("BS Heatmap")) {
                #pragma omp parallel for collapse(2)
                for (int y = 0; y < hm_res; ++y) {
                    for (int x = 0; x < hm_res; ++x) {
                        float s = hm_spot_min + (float)x / (hm_res - 1) * (hm_spot_max - hm_spot_min);
                        float v = hm_vol_min + (float)y / (hm_res - 1) * (hm_vol_max - hm_vol_min);
                        BlackScholes cellBS(s, strike, rate, v, maturity, type);
                        heatmap_values[y * hm_res + x] = (float)cellBS.price();
                    }
                }
                ImPlot::PushColormap(ImPlotColormap_Jet);
                if (ImPlot::BeginPlot("##Heatmap", ImVec2(-1, -1))) {
                    ImPlot::SetupAxes("Spot Price", "Volatility", 0, 0);
                    ImPlot::PlotHeatmap("Price", heatmap_values.data(), hm_res, hm_res, 0, 0, nullptr, 
                                      {hm_spot_min, hm_vol_min}, {hm_spot_max, hm_vol_max});
                    ImPlot::PlotScatter("You", &spot, &volatility, 1);
                    ImPlot::EndPlot();
                }
                ImPlot::PopColormap();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::EndChild();
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}