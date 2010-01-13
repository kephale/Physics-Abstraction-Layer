#ifndef STANDARD_SCENES_H
#define STANDARD_SCENES_H

#include "graph.h"

class Picture: public DemoScene {
public:
	void LoadCentral(const char *img) {
		m_clear_color = SColor(255,255,255,255);
		g_smgr->clear();
		g_gui->clear();
		ITexture *tex = g_driver->getTexture(img);
		if (tex) {
		const core::dimension2d<u32>& size = tex->getSize();
		const core::dimension2d<u32>& ss = g_driver->getScreenSize();
		g_gui->addImage(tex,
			core::position2d<s32>(ss.Width/2 - size.Width/2 ,ss.Height/2 - size.Height/2));
		}
	}
};


class ScriptScene {
public:
	ScriptScene() {
		pds = 0;
	}
	virtual void Run() {
		if (pds) {
			pds->Update();
			pds->Render();
		}
	}
	virtual void Init(ScriptScene *last) = 0;
	DemoScene *pds;
	float time;
};

class SceneSlide : public ScriptScene {
public:
	STRING m_name;
	STRING m_info;
	SceneSlide(STRING name, float t=2, STRING info = "") {
		time = t;
		m_name = name;
		m_info = info;
	}
	Picture splash;
	virtual void Init(ScriptScene *last) {
		if (m_name.length()>2) 
			splash.LoadCentral(m_name.c_str());
		if (m_info.length()>2) {
			const core::dimension2d<u32>& ss = g_driver->getScreenSize();

			size_t a = m_info.length();
			BSTR unicodestr = SysAllocStringLen(NULL, (UINT)a);
			MultiByteToWideChar(CP_ACP, 0, m_info.c_str(), (int)a, unicodestr, (int)a);
			g_gui->addStaticText(unicodestr,rect<s32>(50,ss.Height-50,ss.Width-50,ss.Height-10));
		}
		pds = &splash;
	}
};
#endif