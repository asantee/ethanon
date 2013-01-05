/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#ifndef GS2D_DSHOW_PLAYER_H_
#define GS2D_DSHOW_PLAYER_H_

#include "../../Player.h"
#include <dshow.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )

namespace gs2d {

class DShowPlayer : public Player
{
	friend PlayerPtr CreatePlayer(VideoPtr pVideo, const std::wstring& fileName);

	struct VIDEO_DATA
	{
		VIDEO_DATA()
		{
			pGB = NULL;
			pMC = NULL;
			pVW = NULL;
			pME = NULL;
			pMS = NULL;
			pBA = NULL;
		}
		IGraphBuilder  *pGB;
		IMediaControl  *pMC;
		IVideoWindow   *pVW;
		IMediaEvent    *pME;
		IMediaSeeking  *pMS;
		IBasicAudio    *pBA;
		HWND hWnd;
	} m_data;

	bool m_continue;
	VideoPtr m_pVideo;

	math::Vector2i m_size;
	math::Vector2i m_pos;

	DShowPlayer();
	bool LoadVideo(VideoPtr pVideo, const std::wstring& fileName);

public:
	~DShowPlayer();

	void UpdateVideo();

	bool Play();
	bool Rewind();
	bool Stop();
	bool Pause();

	bool SetVideoPos(const math::Vector2i &pos);
	bool SetVideoSize(const math::Vector2i &size);
	bool SetAudioVolume(const float volume);
	math::Vector2i GetVideoPos() const;
	math::Vector2i GetVideoSize() const ;

	bool SetFullscreen();
	bool IsFullscreen() const;

	void ShowVideo(const bool show);
	bool IsVideoVisible();
	bool IsFinished() const;
	GS_PLAYER_STATUS GetStatus();
};

typedef boost::shared_ptr<DShowPlayer> DShowPlayerPtr;
typedef boost::weak_ptr<DShowPlayer> DShowPlayerWeakPtr;

} // namespace gs2d

#endif