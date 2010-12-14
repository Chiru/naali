//$ HEADER_NEW_FILE $ 
/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_TtsVoice.cpp
 *  @brief  EC_TtsVoice Tts voice component wich allows use Tts function on entity.
*/

#include "StableHeaders.h"

#include "EC_TtsVoice.h"
#include "IModule.h"
#include "Entity.h"

#include "MemoryLeakCheck.h"


EC_TtsVoice::EC_TtsVoice(IModule *module) :
    IComponent(module->GetFramework()),
    voicename(this, "", false),
	voice_(this, "voice", ""),
    message_(this, "message", "")
{
	// Get TTS service
	ttsService_ = framework_->GetService<Tts::TtsServiceInterface>();
	connect(this, SIGNAL(ParentEntitySet()), this, SLOT(RegisterActions()));
	//voice_=Tts::Voices.ES1;
}

EC_TtsVoice::~EC_TtsVoice()
{
}

void EC_TtsVoice::SetMyVoice(const QString voice)
{
	voice_.Set(voice, AttributeChange::LocalOnly);
	//AttributeChanged(voice_,AttributeChange::Local);
	ComponentChanged(AttributeChange::LocalOnly);
	//voice_=voice;
}

QString EC_TtsVoice::GetMyVoice() const
{
    return voice_.Get();
}

void EC_TtsVoice::SpeakMessage(const QString msg, QString voice)
{
	if(!ttsService_)
		ttsService_ = framework_->GetService<Tts::TtsServiceInterface>();
	
	ttsService_->Text2Speech(msg,voice);
}

void EC_TtsVoice::SpeakMessage(const QString msg)
{
	if(!ttsService_)
		ttsService_ = framework_->GetService<Tts::TtsServiceInterface>();
	
	ttsService_->Text2Speech(msg, voice_.Get());
}

void EC_TtsVoice::SpeakMessage()
{
	if(!ttsService_)
		ttsService_ = framework_->GetService<Tts::TtsServiceInterface>();

	ttsService_->Text2Speech(message_.Get(),voice_.Get());
	//ttsService_->text2PHO(message_.Get(),"nombrepho", voice_.Get().toStdString());
	//ttsService_->text2WAV(message_.Get(),"nombrewav", voice_.Get().toStdString());

	//ttsService_->file2Speech("ejemplo.txt",voice_.Get().toStdString());
	//ttsService_->file2PHO("ejemplo.txt","nombrephoejemplo",voice_.Get().toStdString());
	//ttsService_->file2WAV("ejemplo.txt","nombrewavejemplo",voice_.Get().toStdString());
}

void EC_TtsVoice::RegisterActions()
{
    Scene::Entity *entity = GetParentEntity();
    assert(entity);
    if (entity)
        entity->ConnectAction("MousePress", this, SLOT(OnClick()));
}

void EC_TtsVoice::OnClick()
{
	EC_TtsVoice::SpeakMessage();
}