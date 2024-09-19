#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <vector>
#include <cmath>
#include <chrono>

const float PI = 3.14159265f;
using namespace std;

sf::VertexArray createThickArc(float radius, float startAngle, float arcAngle, sf::Vector2f center, float thickness, sf::Color color)
{
    // Number of segments to approximate the arc
    const int segments = 360;
    sf::VertexArray arc(sf::TrianglesStrip, 2 * (segments + 1));

    float startRad = startAngle * PI / 180.0f;
    float step = arcAngle * PI / 180.0f / segments;

    for (int i = 0; i < segments; ++i)
    {
        float angle = startRad + i * step;
        float nextAngle = startRad + (i + 1) * step;

        // Calculate the outer points
        float x1 = center.x + radius * cos(angle);
        float y1 = center.y + radius * sin(angle);
        float x2 = center.x + radius * cos(nextAngle);
        float y2 = center.y + radius * sin(nextAngle);

        // Calculate thickness offset
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        float unitX = dx / length;
        float unitY = dy / length;

        float offsetX = thickness / 2.0f * unitY;
        float offsetY = thickness / 2.0f * (-unitX);

        // Add vertices for the thick arc
        arc[2 * i] = sf::Vertex(sf::Vector2f(x1 + offsetX, y1 + offsetY), color);
        arc[2 * i + 1] = sf::Vertex(sf::Vector2f(x1 - offsetX, y1 - offsetY), color);

        arc[2 * (i + 1)] = sf::Vertex(sf::Vector2f(x2 + offsetX, y2 + offsetY), color);
        arc[2 * (i + 1) + 1] = sf::Vertex(sf::Vector2f(x2 - offsetX, y2 - offsetY), color);
    }

    return arc;
}

sf::VertexArray createThickLine(float x1, float y1, float x2, float y2, float thickness, sf::Color color)
{
    sf::VertexArray line(sf::TrianglesStrip, 4);

    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);
    float unitX = dx / length;
    float unitY = dy / length;

    float offsetX = thickness / 2.0f * unitY;
    float offsetY = thickness / 2.0f * (-unitX);

    line[0] = sf::Vertex(sf::Vector2f(x1 + offsetX, y1 + offsetY), color);
    line[1] = sf::Vertex(sf::Vector2f(x1 - offsetX, y1 - offsetY), color);
    line[2] = sf::Vertex(sf::Vector2f(x2 + offsetX, y2 + offsetY), color);
    line[3] = sf::Vertex(sf::Vector2f(x2 - offsetX, y2 - offsetY), color);

    return line;
}

sf::VertexArray createRotatingLine(sf::Vector2f center, float radius, float angle)
{
    sf::Vector2f end(
        center.x + radius * cos(angle * PI / 180.0f),
        center.y + radius * sin(angle * PI / 180.0f)
    );

    return createThickLine(center.x, center.y, end.x, end.y, 4, sf::Color::Red);
}

int main(int argc, char* argv[])
{
    bool type = true;
    float speed = 1200.0f;

    if (argc > 2) {
        type = string(argv[1]) == "white";
        speed = atoi(argv[2]);
    }

    sf::RenderWindow window(
        sf::VideoMode(150, 150),
        "Borderless Window",
        sf::Style::None
    );

    HWND hwndOver = window.getSystemHandle();
    SetWindowPos(hwndOver, HWND_TOPMOST, 885, 465, 150, 150, SWP_NOSIZE);
    SetLayeredWindowAttributes(hwndOver, RGB(0, 0, 0), 255, LWA_ALPHA);

    sf::RenderTexture renderTexture;
    renderTexture.create(150, 150);

    sf::Vector2f center(75, 75);
    int arcAngle = 40;
    float thickness = 2.0f;

    sf::Texture texture;
    if (!texture.loadFromFile("key.png")) { // Update path as needed
        std::cout << "Failed to load texture from path: key.png" << std::endl;
    }
    sf::Sprite spritePNG(texture);
    spritePNG.setPosition(center - sf::Vector2f(texture.getSize()) / 2.0f);

    srand(time(0));
    int lastAngle = rand() % 360;

    std::vector<sf::VertexArray> arcs;
    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    while (window.isOpen()) {
        arcs.clear();

        int startAngle;
        do {
            startAngle = rand() % 360;
        } while (min(fmod(fabs(startAngle - lastAngle), 360.0f), 360.0f - fmod(fabs(startAngle - lastAngle), 360.0f)) < 80);

        cout << min(fmod(fabs(startAngle - lastAngle), 360.0f), 360.0f - fmod(fabs(startAngle - lastAngle), 360.0f)) << endl;

        lastAngle = startAngle;

        std::vector<sf::VertexArray> arcs;
        arcs.push_back(createThickArc(65, 0, 360, center, thickness, sf::Color::White));
        arcs.push_back(createThickArc(65, startAngle, arcAngle, center, thickness, sf::Color::Black));
        arcs.push_back(createThickArc(68, startAngle, arcAngle, center, thickness, sf::Color::White));
        arcs.push_back(createThickArc(62, startAngle, arcAngle, center, thickness, sf::Color::White));

        if (type) {
            for (int x = 63; x <= 67; x++) {
                arcs.push_back(createThickArc(x, startAngle, 9, center, thickness, sf::Color::White));
            }
        }

        sf::VertexArray connectingLine1 = createThickLine(
            75 + 68 * cos(startAngle * PI / 180.0f),
            75 + 68 * sin(startAngle * PI / 180.0f),
            75 + 62 * cos(startAngle * PI / 180.0f),
            75 + 62 * sin(startAngle * PI / 180.0f),
            thickness,
            sf::Color::White
        );
        sf::VertexArray connectingLine2 = createThickLine(
            75 + 68 * cos((startAngle + arcAngle) * PI / 180.0f),
            75 + 68 * sin((startAngle + arcAngle) * PI / 180.0f),
            75 + 62 * cos((startAngle + arcAngle) * PI / 180.0f),
            75 + 62 * sin((startAngle + arcAngle) * PI / 180.0f),
            thickness,
            sf::Color::White
        );

        while (true) {
            renderTexture.clear();
            window.clear();

            for (const auto& arc : arcs)
            {
                renderTexture.draw(arc);
            }

            sf::VertexArray rotatingLine = createRotatingLine(center, 80, fmod(chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - start).count() / speed * 360, 360.0f));

            renderTexture.draw(connectingLine1);
            renderTexture.draw(connectingLine2);
            renderTexture.draw(spritePNG);
            renderTexture.draw(rotatingLine);

            renderTexture.display();
            sf::Sprite sprite(renderTexture.getTexture());

            window.draw(sprite);
            window.display();

            if (GetAsyncKeyState(VK_SPACE) & 0x80000000) {
                Sleep(21);
                break;
            }   
        }
    }

    return 0;
}
