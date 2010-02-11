// For conditions of distribution and use, see copyright notice in license.txt

/**
 *  @file   OpenSimInventoryDataModel.h
 *  @brief  Data model providing the OpenSim inventory model backend functionality.
 */

#ifndef incl_InventoryModule_OpenSimInventoryDataModel_h
#define incl_InventoryModule_OpenSimInventoryDataModel_h

#include "AbstractInventoryDataModel.h"
#include "RexTypes.h"
#include "WorldStream.h"

#include <QMap>
#include <QPair>

namespace Foundation
{
    class Framework;
    class EventDataInterface;
}

namespace ProtocolUtilities
{
    class InventorySkeleton;
    class InventoryFolderSkeleton;
}

namespace Inventory
{
    class InventoryFolder;

    class OpenSimInventoryDataModel : public AbstractInventoryDataModel
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// @param framework Framework pointer.
        /// @inventory_skeleton Inventory skeleton pointer.
        OpenSimInventoryDataModel(
            Foundation::Framework *framework,
            ProtocolUtilities::InventorySkeleton *inventory_skeleton);

        /// Destructor.
        virtual ~OpenSimInventoryDataModel();

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetFirstChildFolderByName(const QString &searchName) const;

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetChildFolderById(const QString &searchId) const;

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetChildAssetById(const QString &searchId) const;

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetChildById(const QString &searchId) const;

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetOrCreateNewFolder(const QString &id, AbstractInventoryItem &parentFolder,
            const QString &name = "New Folder", const bool &notify_server = true);

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetOrCreateNewAsset(const QString &inventory_id, const QString &asset_id,
            AbstractInventoryItem &parentFolder, const QString &name = "New Asset");

        /// AbstractInventoryDataModel override.
        bool FetchInventoryDescendents(AbstractInventoryItem *item);

        /// AbstractInventoryDataModel override.
        void NotifyServerAboutItemMove(AbstractInventoryItem *item);

        /// AbstractInventoryDataModel override.
        void NotifyServerAboutItemCopy(AbstractInventoryItem *item);

        /// AbstractInventoryDataModel override.
        void NotifyServerAboutItemRemove(AbstractInventoryItem *item);

        /// AbstractInventoryDataModel override.
        void NotifyServerAboutItemUpdate(AbstractInventoryItem *item, const QString &old_name);

        /// AbstractInventoryDataModel override.
        bool OpenItem(AbstractInventoryItem *item);

        /// AbstractInventoryDataModel override.
        void UploadFile(const QString &filename, AbstractInventoryItem *parent_folder);

        /// AbstractInventoryDataModel override.
        void UploadFiles(QStringList &filenames, QStringList &names, AbstractInventoryItem *parent_folder);

        /// AbstractInventoryDataModel override.
        void UploadFilesFromBuffer(QStringList &filenames, QVector<QVector<uchar> > &buffers,
            AbstractInventoryItem *parent_folder);

        /// AbstractInventoryDataModel override.
        /// In OpesimInventoryDataModel this function doesn't perform the actual download.
        /// This just request the asset from server using texture and asset services.
        void DownloadFile(const QString &store_folder, AbstractInventoryItem *selected_item);

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetRoot() const;

        /// AbstractInventoryDataModel override.
        AbstractInventoryItem *GetTrashFolder() const;

        /// @return Pointer to "My Inventory" folder or null if not found.
        InventoryFolder *GetMyInventoryFolder() const;

        /// @return Pointer to "My Inventory" folder or null if not found.
        InventoryFolder *GetOpenSimLibraryFolder() const;

        /// OpenSim inventory uses trash folder. Returns true.
        bool GetUseTrashFolder() const { return true; }

        // OpenSimInventoryDataModel API

        /// Set World Stream.
        /// @param world_stream WorldStream pointer.
        void SetWorldStream(ProtocolUtilities::WorldStreamPtr world_stream) { currentWorldStream_ = world_stream; }

        ///@return True if inventory has pending downloads.
        bool HasPendingDownloadRequests() const { return downloadRequests_.size() > 0; }

        ///@return True if inventory has pending item open requests.
        bool HasPendingOpenItemRequests() const { return openRequests_.size() > 0; }

        /// Handles INVENTORY_DESCENDENTS event.
        /// @param data Event data.
        void HandleInventoryDescendents(Foundation::EventDataInterface *data);

        /// Handles RESOURCE_READY event.
        /// @param data Event data.
//        void HandleResourceReady(Foundation::EventDataInterface *data);

        /// Handles ASSET_READY event for items to be downloaded (i.e. saved to disk).
        /// @param data Event data.
        void HandleAssetReadyForDownload(Foundation::EventDataInterface *data);

        /// Handles ASSET_READY event for items to be opened.
        /// @param data Event data.
        void HandleAssetReadyForOpen(Foundation::EventDataInterface *data);

        /** Uploads a file using HTTP.
         *  @param asset_type_t Asset type.
         *  @param filename Filename.
         *  @param name User-defined name.
         *  @param description User-defined description.
         *  @param folder_id Id of the destination folder for this item.
         *  @return true if successful
         */
        bool UploadFile(
            const RexTypes::asset_type_t &asset_type,
            std::string &filename,
            const std::string &name,
            const std::string &description,
            const RexUUID &folder_id);

        /** Uploads a buffer using HTTP.
         *  @param asset_type_t Asset type.
         *  @param filename Filename (used to decide asset type)
         *  @param name User-defined name.
         *  @param description User-defined description.
         *  @param folder_id Id of the destination folder for this item.
         *  @param data buffer
         *  @return true if successful
         */

        bool UploadBuffer(
            const RexTypes::asset_type_t &asset_type,
            const std::string &filename,
            const std::string &name,
            const std::string &description,
            const RexUUID &folder_id,
            const QVector<uchar>& buffer);

        /// @return Does asset uploader have upload capability set.
        bool HasUploadCapability() const { return uploadCapability_ != ""; }

        /// Sets the upload capability URL.
        /// @param url Capability URL.
        void SetUploadCapability(const std::string &url) { uploadCapability_ = url; }

        /// Utility function for create name for asset from filename.
        /// @param filename Filename.
        /// @return Filename without the file extension.
        QString CreateNameFromFilename(QString filename);

        typedef QMap<QPair<request_tag_t, QString>, QString> AssetRequestMap;

    signals:
        /// This signal is emitted to show notification on the window.
        /// @param message Message to be shown.
        /// @param int duration Duration in milliseconds.
        void Notification(const QString &message, int duration);

    private:
        Q_DISABLE_COPY(OpenSimInventoryDataModel);

        /// Utility function for creating new folders from the folder skeletons. Used recursively.
        /// @param parent_folder Parent folder.
        /// @param folder_skeleton Folder skeleton for the folder to be created.
        void CreateNewFolderFromFolderSkeleton(
            InventoryFolder *parent_folder,
            ProtocolUtilities::InventoryFolderSkeleton *folder_skeleton);

        /// Creates the tree model data for inventory.
        /// @param inventory_skeleton OpenSim inventory skeleton.
        void SetupModelData(ProtocolUtilities::InventorySkeleton *inventory_skeleton);

        /// Used by UploadFiles.
        void ThreadedUploadFiles(QStringList &filenames, QStringList &item_names);

        /// Used by UploadBuffers.
        //void ThreadedUploadBuffers(StringList filenames, std::vector<std::vector<u8> > buffers);
        void ThreadedUploadBuffers(QStringList filenames, QVector<QVector<uchar> > buffers);

        /// Creates NewFileAgentInventory XML message.
        std::string CreateNewFileAgentInventoryXML(
            const std::string &asset_type,
            const std::string &inventory_type,
            const std::string &folder_id,
            const std::string &name,
            const std::string &description);

        /// Creates all the reX-spesific asset folders to the inventory.
        void CreateRexInventoryFolders();

        /// Framework pointer
        Foundation::Framework *framework_;

        /// The root folder.
        InventoryFolder *rootFolder_;

        /// World Library owner id.
        QString worldLibraryOwnerId_;

        /// Upload capability URL.
        std::string uploadCapability_;

        /// Pointer to WorldStream
        ProtocolUtilities::WorldStreamPtr currentWorldStream_;

        /// Download request map.
        AssetRequestMap downloadRequests_;

        /// Item open request map.
        AssetRequestMap openRequests_;
    };
}

#endif
