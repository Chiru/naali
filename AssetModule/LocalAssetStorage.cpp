// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "MemoryLeakCheck.h"
#include "LocalAssetStorage.h"
#include "LocalAssetProvider.h"
#include "AssetAPI.h"

#include <QFileSystemWatcher>
#include <QDir>
#include <utility>

namespace Asset
{

LocalAssetStorage::LocalAssetStorage()
//:changeWatcher(0)
{
}

LocalAssetStorage::~LocalAssetStorage()
{
    RemoveWatcher();
}

QString LocalAssetStorage::GetFullPathForAsset(const QString &assetname, bool recursiveLookup)
{
    QDir dir(GuaranteeTrailingSlash(directory) + assetname);
    if (boost::filesystem::exists(dir.absolutePath().toStdString()))
        return directory;

    if (!recursive || !recursiveLookup)
        return "";

    try
    {
        boost::filesystem::recursive_directory_iterator iter(directory.toStdString());
        boost::filesystem::recursive_directory_iterator end_iter;
        // Check the subdir
        for(; iter != end_iter; ++iter)
        {
            QDir dir(GuaranteeTrailingSlash(iter->path().string().c_str()) + assetname);
            if (!fs::is_regular_file(iter->status()) && boost::filesystem::exists(dir.absolutePath().toStdString()))
                return iter->path().string().c_str();
        }
    }
    catch (...)
    {
    }

    return "";
}

QString LocalAssetStorage::GetFullAssetURL(const QString &localName)
{    
    return BaseURL() + AssetAPI::ExtractFilenameFromAssetRef(localName);
}

void LocalAssetStorage::SetupWatcher()
{
    /* This watcher is not used for now. -jj. will be removed.
    if (changeWatcher) // Remove the old watcher if one exists.
        RemoveWatcher();

    changeWatcher = new QFileSystemWatcher();

    // Add a watcher to listen to if the directory contents change.
    changeWatcher->addPath(directory);

    // Add a watcher to each file in the directory.
    QDir dir(directory);
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    foreach(QFileInfo i, files)
        changeWatcher->addPath(i.absoluteFilePath());

    ///\todo The QFileSystemWatcher is severely lacking in functionality. Replace the above with some custom method that can tell
    /// which files change.
    */
}

void LocalAssetStorage::RemoveWatcher()
{
    /* This watcher is not used for now. -jj. will be removed.
    delete changeWatcher;
    changeWatcher = 0;
    */
}

} // ~Asset

