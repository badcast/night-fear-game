#pragma once
#include <typeinfo>

#include "framework.h"

using namespace std;
namespace RoninEngine::UI {
   class UC;
   class GUI;

   void InitalizeControls();
   void Free_Controls();
   void ResetControls();

   typedef void (*ui_callback)(const uint8_t& id, void* userdata);


   enum ControlType : std::uint8_t {
      _UC,
      CTEXT,
      CBUTTON,
      CEDIT,
      CHSLIDER,
      CVSLIDER,
      CIMAGEANIMATOR,
      CTEXTRAND,
      CIMAGE
   };
   struct RenderData {
         Rect_t rect;
         uint8_t options;
         int8_t id;
         int8_t parentId;
         string text;
         void* resources;
         UC* prototype;
         list<uint8_t> childs;
   };

   typedef void (*ui_render_callback)(GUI* gui, RenderData& data,
                                      SDL_Renderer* render, bool* hover);

   list<UC*> nativeControls();

   UC* findControl(ControlType control);


   class UC {
      public:
         virtual const ControlType id() const{ return ControlType::_UC; }
         virtual bool render_control(GUI* gui, RenderData& data,
                                     SDL_Renderer* render, bool* hover);
         virtual void destructor(void* resources);
   };

   class CText : public UC {
      public:
         const ControlType id() const{ return CTEXT; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };

   class CButton : public UC {
      public:
         const ControlType id() const{ return CBUTTON; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };

   class CEdit : public UC {
      public:
         const ControlType id() const{ return CEDIT; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };

   class CHorizontalSlider : public UC {
      public:
         const ControlType id() const{ return CHSLIDER; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };

   class CVerticalSlider : public UC {
      public:
         const ControlType id() const{ return CVSLIDER; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };

   class CImage : public UC {
      public:
         const ControlType id() const{ return CIMAGE; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };

   class CImageAnimator : public UC {
      public:
         const ControlType id() const { return CIMAGEANIMATOR; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };

   class CTextRandomizerDisplay : public UC {
      public:
         const ControlType id() const{ return CTEXTRAND; }
         bool render_control(GUI* gui, RenderData& data, SDL_Renderer* render,
                             bool* hover);
         void destructor(void* resources);
   };
}  // namespace RoninEngine::UI
