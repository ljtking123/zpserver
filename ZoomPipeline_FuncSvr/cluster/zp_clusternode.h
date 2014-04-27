#ifndef ZP_CLUSTERNODE_H
#define ZP_CLUSTERNODE_H

#include <QObject>
#include <QHostAddress>
#include <QDateTime>
#include "../pipeline/zp_pltaskbase.h"
#include "cross_svr_messages.h"
namespace ZP_Cluster{
	class zp_ClusterTerm;
	/**
	 * @brief This class stand for a remote server.
	 * when local server establish a connection between itself and remote svr,
	 * a zp_ClusterNode object will be created.
	 */
	class zp_ClusterNode : public ZPTaskEngine::zp_plTaskBase
	{
		Q_OBJECT
	public:
		explicit zp_ClusterNode(zp_ClusterTerm * pTerm, QObject * psock,QObject *parent = 0);
		int run();
		bool bTermSet;
		//!deal at most m_nMessageBlockSize messages per deal_message();
		static const int m_nMessageBlockSize = 8;
		//push new binary data into queue
		int push_new_data(const  QByteArray &  dtarray);
		//!deal one message, affect m_currentRedOffset,m_currentMessageSize,m_currentHeader
		//!return bytes Used.
		int filter_message(const QByteArray &, int offset);
		//!in Trans-Layer, it does nothing.
		int deal_current_message_block();

		QDateTime lastActiveTime(){ return m_last_Report;}
		void CheckHeartBeating();

	public:
		QString termName(){return m_strTermName;}
		QHostAddress addrPublish(){return m_addrPublish;}
		int portPublish() {return m_nPortPublish;}
		QObject * sock() { return m_pSock;}
	protected:
		zp_ClusterTerm * m_pTerm;
		//Client socket handle of this connection
		QObject * m_pSock;
		//the data members.
		QString m_strTermName;		//the Terminal's name
		QHostAddress m_addrPublish;	//The publish address for other terms to connect to
		int m_nPortPublish;			//The publish port for other terms to connect to

		//Data Process
		//The raw data queue and its mutex
		QList<QByteArray> m_list_RawData;
		QMutex m_mutex_rawData;
		//The current Read Offset, from m_list_RawData's beginning
		int m_currentReadOffset;
		//Current Message Offset, according to m_currentHeader
		int m_currentMessageSize;
		//Current un-procssed message block.for large blocks,
		//this array will be re-setted as soon as some part of data has been
		//dealed, eg, send a 200MB block, the 200MB data will be splitted into pieces
		QByteArray m_currentBlock;

		CROSS_SVR_MSG::tag_header m_currentHeader;

		QDateTime m_last_Report;

	signals:
		void evt_SendDataToClient(QObject * objClient,const QByteArray &  dtarray);
		void evt_BroadcastData(QObject * objFromClient,const QByteArray &  dtarray);
		void evt_close_client(QObject * objClient);
		void evt_Message (QObject * psource,const QString &);
	};
}
#endif // ZP_CLUSTERNODE_H