#include "pch.h"

namespace RoninEngine::Runtime {
SpriteRenderer::SpriteRenderer() : SpriteRenderer("Sprite Renderer") {}

SpriteRenderer::SpriteRenderer(const string& name) : Renderer(name) {
    sprite = nullptr;
    texture = nullptr;
    flip = Vec2::one;
    SpriteRenderType renderType = SpriteRenderType::Simple;
    SpriteRenderTile tileRenderPresent = SpriteRenderTile::Fixed;
    memset(&color, ~0, sizeof(color));
}

SpriteRenderer::SpriteRenderer(SpriteRenderer& ref) {
    this->name() = ref.name();
    this->sprite = ref.sprite;
    this->color = ref.color;
    this->flip = ref.flip;
    this->size = ref.size;
    this->zOrder = ref.zOrder;
    this->texture = ref.texture;
    this->renderType = ref.renderType;
    this->tileRenderPresent = ref.tileRenderPresent;
}

SpriteRenderer::~SpriteRenderer() {}

Vec2 SpriteRenderer::GetSize() { return Vec2::Abs(this->size) * squarePerPixels; }

void SpriteRenderer::setSprite(Sprite* sprite) {
    if (this->sprite == nullptr && (!this->size.x || !this->size.y)) {
        this->size.x = sprite->width() / squarePerPixels;
        this->size.y = sprite->height() / squarePerPixels;
    }
    this->sprite = sprite;
}
Sprite* SpriteRenderer::getSprite() { return this->sprite; }

void SpriteRenderer::Render(Render_info* render) {
    Rect_t& _srcRect = render->src;
    Rectf_t& _dstRect = render->dst;
    uint16_t x, y;
    SDL_Rect dest;

    if (sprite && sprite->width() && sprite->height()) {
        switch (this->renderType) {
            case SpriteRenderType::Simple:
                if (!texture) {
                    texture = sprite->texture;
                }
                _srcRect.w = abs(this->size.x) * squarePerPixels;
                _srcRect.h = abs(this->size.y) * squarePerPixels;
                _dstRect.w = Mathf::abs(this->size.x);
                _dstRect.h = Mathf::abs(this->size.y);

                break;
            case SpriteRenderType::Tile: {
                _srcRect.w = abs(this->size.x) * squarePerPixels;
                _srcRect.h = abs(this->size.y) * squarePerPixels;
                _dstRect.w = this->size.x;
                _dstRect.h = this->size.y;

                if (this->tileRenderPresent == SpriteRenderTile::Fixed) {
                    _srcRect.w = (_srcRect.w / sprite->width()) * sprite->width();
                    _srcRect.h = (_srcRect.h / sprite->height()) * sprite->height();
                }

                if (!texture) {
                    GC::gc_alloc_texture(&texture, _srcRect.w, _srcRect.h, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET);

                    if (texture == nullptr) Application::fail("Texture create fail");

                    SDL_SetRenderTarget(render->renderer, texture->native());

                    dest.w = sprite->width();
                    dest.h = sprite->height();

                    _srcRect.x = _srcRect.w / dest.w;
                    _srcRect.y = _srcRect.h / dest.h;

                    // render tile
                    switch (this->tileRenderPresent) {
                        case SpriteRenderTile::Fixed: {
                            for (x = 0; x < _srcRect.x; ++x) {
                                for (y = 0; y < _srcRect.y; ++y) {
                                    dest.x = x * dest.w;
                                    dest.y = y * dest.h;
                                    SDL_RenderCopy(render->renderer, sprite->texture->native(), (SDL_Rect*)&sprite->m_rect,
                                                   (SDL_Rect*)&dest);
                                }
                            }
                            break;
                        }
                        case SpriteRenderTile::Place: {
                            for (x = 0; x < _srcRect.x; ++x) {
                                for (y = 0; y < _srcRect.y; ++y) {
                                    dest.x = x * dest.w;
                                    dest.y = y * dest.h;

                                    SDL_RenderCopy(render->renderer, sprite->texture->native(), (SDL_Rect*)&sprite->m_rect,
                                                   (SDL_Rect*)&dest);
                                }
                            }
                            // place remained
                            for (x = 0, dest.y = _srcRect.h / dest.h * dest.h, dest.h = size.y - dest.y; x < _srcRect.x; ++x) {
                                dest.x = x * dest.w;
                                SDL_RenderCopy(render->renderer, sprite->texture->native(), (SDL_Rect*)&sprite->m_rect,
                                               (SDL_Rect*)&dest);
                            }
                            ++_srcRect.y;
                            for (y = 0, dest.x = _srcRect.w / dest.w * dest.w, dest.h = sprite->height(),
                                dest.w = size.x - dest.x;
                                 y < _srcRect.y; ++y) {
                                dest.y = y * dest.h;
                                SDL_RenderCopy(render->renderer, sprite->texture->native(), (SDL_Rect*)&sprite->m_rect,
                                               (SDL_Rect*)&dest);
                            }

                            break;
                        }
                    }
                    SDL_SetRenderTarget(render->renderer, nullptr);
                }
            } break;
        }
    }

    _dstRect.w *= flip.x;
    _dstRect.h *= flip.y;

    render->texture = texture;
}
}  // namespace RoninEngine::Runtime
