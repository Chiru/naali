// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "ChatSessionWidget.h"
#include "UiDefines.h"

namespace CommunicationUI
{
    ChatSessionWidget::ChatSessionWidget(QWidget *parent, Communication::ChatSessionInterface *chat_session, QString &my_name, QString &his_name)
        : QWidget(parent),
          chat_session_(chat_session),
          my_name_(my_name),
          his_name_(his_name)
    {
        chat_session_ui_.setupUi(this);
        ShowMessageHistory(chat_session_->GetMessageHistory());

        connect(chat_session_, SIGNAL( MessageReceived(const Communication::ChatMessageInterface&)),
                this, SLOT( MessageReceived(const Communication::ChatMessageInterface&)));
        connect(chat_session_ui_.sendMessageLineEdit, SIGNAL( returnPressed() ),
                this, SLOT( SendMessage() ));
        connect(chat_session_ui_.closePushButton, SIGNAL( clicked() ),
                this, SLOT( CloseConversation() ));
    }

    ChatSessionWidget::~ChatSessionWidget()
    {

    }

    void ChatSessionWidget::MessageReceived(const Communication::ChatMessageInterface &message)
    {
        QString html("<span style='color:#828282;'>[");
        html.append(UiDefines::TimeStampGenerator::FormatTimeStamp(message.GetTimeStamp()));
		html.append("]</span> <span style='color:#2133F0;'>");
        html.append(message.GetOriginator()->GetName());
		html.append("</span><span style='color:black;'>: ");
		html.append(message.GetText());
		html.append("</span>");
		MessageToConversation(html);
    }

	void ChatSessionWidget::SendMessage()
	{
		QString message(chat_session_ui_.sendMessageLineEdit->text());
	    chat_session_ui_.sendMessageLineEdit->clear();
		chat_session_->SendMessage(message);

		QString html("<span style='color:#828282;'>[");
		html.append(UiDefines::TimeStampGenerator::GenerateTimeStamp());
		html.append("]</span> <span style='color:#C21511;'>");
		html.append(my_name_);
		html.append("</span><span style='color:black;'>: ");
		html.append(message);
		html.append("</span>");
		MessageToConversation(html);
	}

	void ChatSessionWidget::ShowMessageHistory(Communication::ChatMessageVector messageHistory) 
	{
		Communication::ChatMessageVector::const_iterator itrHistory;
		for (itrHistory = messageHistory.begin(); itrHistory!=messageHistory.end(); itrHistory++)
		{
			Communication::ChatMessageInterface *msg = (*itrHistory);
			QString html("<span style='color:#828282;'>[");
			html.append(UiDefines::TimeStampGenerator::FormatTimeStamp(msg->GetTimeStamp()));
			html.append("]</span> <span style='color:#2133F0;'>");
			html.append(msg->GetOriginator()->GetName());
			//! @todo check if the originator is the current user
			html.append("</span><span style='color:black;'>: ");
			html.append(msg->GetText());
			html.append("</span>");
			MessageToConversation(html);
		}
	}

	void ChatSessionWidget::ContactStateChanged(const QString &status, const QString &message)
	{
		QString html("<span style='color:#828282;'>[");
		html.append(UiDefines::TimeStampGenerator::GenerateTimeStamp());
		html.append("] ");
		html.append(his_name_);
		html.append(" changed status to ");
		html.append(status);
		if ( message.size() > 0 )
		{
			html.append(": ");
			html.append(message);
		}
		html.append("</span>");
		MessageToConversation(html);
	}

    void ChatSessionWidget::CloseConversation()
    {
        emit Closed(his_name_);
    }
}