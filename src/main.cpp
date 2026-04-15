#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <array>
#include <memory>
#include <algorithm>


int getIndex(int x, int y, int width){
    return x + y * width;
}

int main(){
    const unsigned w = 800;
    const unsigned h = 600;
    const unsigned pixel_num = w * h;

    float diffusionRate = 0.25f;
    float dissipation = 0.00f;
    int brushRadius = 2;
    int simulationSpeed = 1;

    bool showMenu = false;

    auto pixelData = std::make_unique<std::array<std::uint8_t, pixel_num * 4>>();

    for(unsigned i = 0; i < pixel_num; i++){
        (*pixelData)[i*4] = 0;
        (*pixelData)[i*4 + 1] = 0;
        (*pixelData)[i*4 + 2] = 0;
        (*pixelData)[i*4 + 3] = 255; 
    }

    sf::Texture texture;

    if(!texture.resize({w, h})) return -1;

    sf::Sprite sprite(texture);

    auto heatGrid = std::make_unique<std::array<double, pixel_num>>();
    auto nextHeatGrid = std::make_unique<std::array<double, pixel_num>>();

    heatGrid->fill(0.0);
    nextHeatGrid->fill(0.0);

    sf::RenderWindow window(sf::VideoMode({w, h}), "Thermal conduction");

    if(!ImGui::SFML::Init(window)) return -1;

    sf::Clock deltaClock;

    while(window.isOpen()){
        while(const std::optional event = window.pollEvent()){
            ImGui::SFML::ProcessEvent(window, *event);

            if(event->is<sf::Event::Closed>())
                window.close();
            
            if(const auto& keyPressed = event->getIf<sf::Event::KeyPressed>()){
                if(keyPressed->scancode == sf::Keyboard::Scancode::R){
                    heatGrid->fill(0.0);
                }

                if(keyPressed->code == sf::Keyboard::Key::Escape){
                    showMenu = !showMenu;
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        if(showMenu){
            ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_FirstUseEver);
            ImGui::Begin("Settings", &showMenu, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::SliderFloat("Diffusion", &diffusionRate, 0.0f, 0.25f);
            ImGui::SliderFloat("Dissipation", &dissipation, 0.0f, 0.01f, "%.4f");
            ImGui::SliderInt("Brush size", &brushRadius, 1, 50);
            ImGui::SliderInt("Simulation speed", &simulationSpeed, 0, 20);

            ImGui::End();
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !ImGui::GetIO().WantCaptureMouse){
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            for(int dy = -brushRadius; dy <= brushRadius; dy++){
                for(int dx = -brushRadius; dx <= brushRadius; dx++){
                    if(dx*dx + dy*dy <= brushRadius*brushRadius){
                        int nx = mousePos.x + dx;
                        int ny = mousePos.y + dy;

                        if(nx >= 1 && nx < (int)w-1 && ny >= 1 && ny < (int)h-1){
                            (*heatGrid)[getIndex(nx, ny, w)] += 500.0;
                        }
                    }
                }
            }

            
        }

        for(int step = 0; step < simulationSpeed; step++){
            for(unsigned y = 1; y < h-1; y++){
                for(unsigned x = 1; x < w-1; x++){
                    int i = getIndex(x, y, w);

                    double oldTemp = (*heatGrid)[i];
                    double left = (*heatGrid)[getIndex(x-1, y, w)];
                    double right = (*heatGrid)[getIndex(x+1, y, w)];
                    double up = (*heatGrid)[getIndex(x, y-1, w)];
                    double down = (*heatGrid)[getIndex(x, y+1, w)];

                    double newTemp = oldTemp + diffusionRate * (left + right + up + down - 4 * oldTemp);
                    
                    newTemp *= (1 - dissipation);

                    (*nextHeatGrid)[i] = newTemp;
                }
            }

            std::swap(heatGrid, nextHeatGrid);
        }

        for(unsigned i = 0; i < pixel_num; i++){
            double temp = (*heatGrid)[i];

            double clampedTemp = std::clamp(temp, 0.0, 255.0);

            std::uint8_t r = static_cast<std::uint8_t>(std::clamp(clampedTemp*2, 0.0, 255.0));
            std::uint8_t g = static_cast<std::uint8_t>(std::clamp(clampedTemp*2 - 255.0, 0.0, 255.0));
            std::uint8_t b = 0;

            (*pixelData)[i*4] = r;
            (*pixelData)[i*4 + 1] = g;
            (*pixelData)[i*4 + 2] = b;
        }

        texture.update(pixelData->data());

        window.clear();
        window.draw(sprite);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}