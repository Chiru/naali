var SceneAPI = framework.Scene();

SceneAPI.DefaultWorldSceneChanged.connect(SceneChanged);

function SceneChanged(scene)
{
	print("Default scene changed!");
	if(scene==null)
		return;

	var	cameraEntity = scene.GetEntityByNameRaw("AvatarCamera");
	
	if(cameraEntity==null)
		cameraEntity = scene.GetEntityByNameRaw("FreeLookCamera");

	if(cameraEntity==null)
		return;

	camera = cameraEntity.GetComponentRaw("EC_OgreCamera");
	
	if(camera==null)
		return;

	print("Setting camera active!");
	camera.SetActive();
}
