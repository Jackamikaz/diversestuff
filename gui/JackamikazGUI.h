#ifndef __JACKAMIKAZ_GUI_H__
#define __JACKAMIKAZ_GUI_H__

#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>
#include <allegro5\events.h>
#include <allegro5\utf8.h>
#include <list>
#include <string>

#pragma warning(disable:4250)

namespace jmg
{
	class Base {
	private:
		void redraw(int origx, int origy);
		void cascadeDraw(int origx, int origy, bool parentNeedsIt = false);
		bool cascadeHandleEvent(const ALLEGRO_EVENT& event);

		Base * mParent;
		std::list<Base*> mChildren;
		bool mNeedsRedraw;

		bool mRemoveMe;
	protected:
		virtual void draw(int origx, int origy);
		virtual bool handleEvent(const ALLEGRO_EVENT& event);
	public:
		int mRelx;
		int mRely;
		ALLEGRO_COLOR mColor;
		bool mDeleteMe;

		Base(int relx=0, int rely=0, ALLEGRO_COLOR color=al_map_rgb(255,255,255));
		virtual ~Base() {}

		inline Base* parent() { return mParent; }

		bool has(const Base* child) const;

		void addChild(Base* child);
		void remove();

		void baseDraw();
		bool baseHandleEvent(const ALLEGRO_EVENT& event);

		int calcOrigX() const;
		int calcOrigY() const;

		void needsRedraw(int depth = 0);
	};

	class WallPaper : public Base{
	public:
		WallPaper(const ALLEGRO_COLOR& color);
		void draw(int, int);
	};

	class Image : public Base {
	public:
		enum PreRenderedImage {
			CROSS,
			ARROW_UP,
			ARROW_DOWN,
			ARROW_LEFT,
			ARROW_RIGHT
		};

		static ALLEGRO_BITMAP* getImage(PreRenderedImage img);

		ALLEGRO_BITMAP* mImage;

		Image(const ALLEGRO_COLOR& color = al_map_rgb(255,255,255));
		Image(const char* file, const ALLEGRO_COLOR& color = al_map_rgb(255, 255, 255));
		Image(ALLEGRO_BITMAP* bitmap, const ALLEGRO_COLOR& color = al_map_rgb(255, 255, 255));
		Image(PreRenderedImage image, const ALLEGRO_COLOR& color = al_map_rgb(255, 255, 255));

		void draw(int, int);
	};

	class Rectangle {
	public:
		Rectangle();
		Rectangle(int w, int h);
		int mWidth;
		int mHeight;
	};

	class InteractiveRectangle : public virtual Rectangle, public virtual Base {
	public:
		InteractiveRectangle();
		InteractiveRectangle(int w, int h);
		bool isPointInside(int px, int py);
		bool catchMouse(const ALLEGRO_EVENT& event, int button = 1, ALLEGRO_EVENT_TYPE evType = ALLEGRO_EVENT_MOUSE_BUTTON_DOWN);
	};

	class Moveable : public InteractiveRectangle {
	private:
		bool mHeld;
		int mDx;
		int mDy;
	public:
		Base * mTarget;
		unsigned int mButton;

		Moveable();
		Moveable(int w, int h);
		bool handleEvent(const ALLEGRO_EVENT& event);
	};

	class DrawableRectangle : public virtual InteractiveRectangle {
	public:
		DrawableRectangle();
		DrawableRectangle(int w, int h);
		void draw(int origx, int origy);

		unsigned char mOutline;
	};

	class MoveableRectangle : public Moveable, public DrawableRectangle {
	public:
		MoveableRectangle(int w,int h);
	};

	class Button : public DrawableRectangle {
	private:
		bool mHovering;
		bool mClicking;

	public:
		Button();
		Button(int w, int h);

		void draw(int origx, int origy);
		bool handleEvent(const ALLEGRO_EVENT& event);

		void(*mCallback)(void*);
		void* mCallbackArgs;
	};

	class Label : public Base {
	protected:
		ALLEGRO_USTR* mValue;
		bool mEditing;

	public:
		ALLEGRO_FONT* mFont;
		int mWidth;

		inline int getCharAt(int pos) const { return al_ustr_get(mValue, al_ustr_offset(mValue, pos)); }

		void setValue(const char* val);
		void setValue(const char16_t* val);

		Label(const char* val = "");
		Label(const char16_t* val);
		~Label();

		void draw(int origx, int origy);
	};

	class Text : public Label {
	private:
		void insert(int keycode);
		int getTextIndexFromCursorPos(int fromx, int fromy) const;
		void getCursorPosFromTextIndex(int pos, int* posx, int* posy) const;
		
		int cursorXRef;
		void resetCursorXRef();

		bool mClicking;

		bool handleCursorPosEvents(const ALLEGRO_EVENT& event);
		bool collapseSelection();
		void confirmEditing();
	protected:
		bool mIsNumeric;
	public:
		int mTextPos;
		int mSelectionPos;

		void(*mEditCallback)(void*);
		void* mEditCallbackArgs;

		unsigned char mMaxDigits;
		unsigned char mMaxDecimals;
		bool mPositiveOnly;

		Text(const char* val = "");
		Text(const char16_t* val);

		void draw(int origx, int origy);
		bool handleEvent(const ALLEGRO_EVENT& event);
	};

	class Numeric : public Text {
	public:
		Numeric(unsigned char maxDigits, unsigned char maxDecimals, bool positiveOnly);

		inline int getAsInt() const { return std::stoi(std::string(al_cstr(mValue))); }
		inline float getAsFloat() const { return std::stof(std::string(al_cstr(mValue))); }
		inline double getAsDouble() const { return std::stod(std::string(al_cstr(mValue))); }

		template<typename T>
		inline void setFrom(T& val) { setValue(std::to_string(val).c_str()); }
	};

	class Window : public DrawableRectangle {
	public:
		MoveableRectangle mMover;
		Button mBtnClose;
		Image mBtnImage;
		Label mCaption;

		Window(int w, int h, const char* caption = "Window");
		bool handleEvent(const ALLEGRO_EVENT& event);

		void open();
		void close();

		void setParent(Base* parent, bool startsOpen);
	};

	ALLEGRO_FONT* fetchDefaultFont();
}

#endif
