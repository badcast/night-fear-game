#include "Gizmos.h"

#include "pch.h"

namespace RoninEngine::Runtime {
constexpr float maxWorldScalar = 10000;

Color Gizmos::color;
float Gizmos::angle;

void Gizmos::DrawLine(Vec2 a, Vec2 b) {
    if (!Camera::mainCamera()) return;

    Vec2 p = Camera::mainCamera()->transform()->position();
    Vec2 dst;

    auto display = Application::display();

    // dst.x = ((rect.w - dst.w) / 2.0f - (point->x + sourcePoint->x) *
    // squarePerPixels); dst.y = ((rect.h - dst.h) / 2.0f + (point->y -
    // sourcePoint->y) * squarePerPixels);

    // scalars

    dst.x = display.w / 2.f;
    dst.y = display.h / 2.f;
    a.x = dst.x - (p.x - a.x) * squarePerPixels;
    a.y = dst.y + (p.y - a.y) * squarePerPixels;
    b.x = dst.x - (p.x - b.x) * squarePerPixels;
    b.y = dst.y + (p.y - b.y) * squarePerPixels;

    SDL_SetRenderDrawColor(Application::GetRenderer(), color.r, color.g,
                           color.b, color.a);
    SDL_RenderDrawLineF(Application::GetRenderer(), a.x, a.y, b.x, b.y);
}
void drawBox() {
    Vec2 a, b;

    a.x = squarePerPixels;
    a.y = squarePerPixels;
    b.x = -squarePerPixels;
    b.y = squarePerPixels;
    Gizmos::DrawLine(std::move(a), std::move(b));

    a.x = squarePerPixels;
    a.y = -squarePerPixels;
    b.x = -squarePerPixels;
    b.y = -squarePerPixels;
    Gizmos::DrawLine(std::move(a), std::move(b));

    a.x = -squarePerPixels;
    a.y = squarePerPixels;
    b.x = -squarePerPixels;
    b.y = -squarePerPixels;
    Gizmos::DrawLine(std::move(a), std::move(b));

    a.x = squarePerPixels;
    a.y = squarePerPixels;
    b.x = squarePerPixels;
    b.y = -squarePerPixels;
    Gizmos::DrawLine(std::move(a), std::move(b));
}

void Gizmos::DrawPosition(const Vec2 &origin){
    Vec2 a = Vec2::zero, b = Vec2::zero;

    // Draw Line H
    b = a = origin;
    a.x -= maxWorldScalar;
    b.x += maxWorldScalar;
    DrawLine(std::move(a), std::move(b));

    // Draw Line V
    b = a = origin;
    a.y -= maxWorldScalar;
    b.y += maxWorldScalar;
    DrawLine(std::move(a), std::move(b));
}

void Gizmos::Draw2DWorldSpace(const Vec2 & origin, int depth) {
    int i;
    Vec2 dest1, dest2;
    dest1 = dest2 = origin;
    color.r = 0;
    color.g = 255;
    color.b = 0;
    color.a = 1;

    // Draw H and V position
    DrawPosition(std::move(origin));

    for (i = 0; i < depth; ++i) {
        dest1 += Vec2::one;
        DrawPosition(std::move(dest1));

        dest2 += Vec2::minusOne;
        DrawPosition(std::move(dest2));
    }
}

void Gizmos::DrawNavMesh(AIPathFinder::NavMesh* navMesh) {
    Vec2 lastPoint;
    Vec2 a, b;
    AIPathFinder::Neuron* p;
    AIPathFinder::Disposition p1, p2;
    SDL_DisplayMode display;

    if(Camera::mainCamera() == nullptr){
         return;
    }

    display = Application::display();

    color.r = 53;
    color.g = 0;
    color.b = 128;
    color.a = 255;

    navMesh->neuron(Camera::mainCamera()->ScreenToWorldPoint(Vec2(0, 0)), p1);
    navMesh->neuron(
        Camera::mainCamera()->ScreenToWorldPoint(Vec2(display.w, display.h)),
        p2);
    int x = p1.x, y = p1.y;
    while (x < p2.x) {
        while (y < p2.y) {
            p = navMesh->neuron(x, y);
            lastPoint = navMesh->PointToWorldPosition(x, y);
            color.r = !p || p->locked() ? 255 : 53;
            color.g = p && p->total() ? 100 : 0;

            // Draw Line H
            b = a = lastPoint;
            a.x -= 0.03f;
            b.x += 0.03f;
            DrawLine(std::move(a), std::move(b));

            // Draw Line V
            b = a = lastPoint;
            a.y -= 0.03f;
            b.y += 0.03f;
            DrawLine(std::move(a), std::move(b));
            ++y;
        }
        y = p1.y;
        ++x;
    }
}

void Gizmos::DrawTriangle(Vec2 pos, float base, float height) {
    Vec2 a, b;
    pos.y -= height / 2;
    a = b = pos;
    //  base /= 2.f;
    a.x -= base;
    b.x += base;
    pos.y += height;

    // draw base
    DrawLine(std::move(a), std::move(b));

    // draw left side
    DrawLine(std::move(a), std::move(pos));

    // draw right side
    DrawLine(std::move(b), std::move(pos));
}

float Gizmos::square_triangle(float base, float height) {
    return base * height / 2;
}
float Gizmos::square(float x) { return x * x; }
float Gizmos::square_rectangle(float a, float b) { return a * b; }
float Gizmos::square_circle(float radius) {
    /*
     *      get the square circle
     *      S = PI * r^2
     *      S = PI * (d/2)^2
     *      S = (PI * d^2) / 4
     *
     */
    return Mathf::PI * Mathf::pow2(radius);
}
float Gizmos::square_mesh(list<Vec2>&& vecs) {
    /*
     *    get the square a customize figure
     *    S = square
     *    S1 = cell square
     *    n = closed cell
     *    r = partially closed cell
     *    S = S1 * (n + 1/2 * r)
     *
     */

    float S = -1;

    // TODO: написать алгоритм измерение площади произвольным фигурам
    throw std::runtime_error("algorithm is not implemented");
    return S;
}
}  // namespace RoninEngine::Runtime
