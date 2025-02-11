fileIn "file-utils.ms"
fileIn "vertex-types.ms"
fileIn "export-scene-utils.ms"

shouldCopyTextures = true
acceptedExtensions = #(".dds", ".jpg", ".jpeg", ".png", ".bmp")

fn exportTextureFile texTypeName texMap = (
	local texturePath = (openBitMap texMap.bitmap.filename).filename
	local textureName = filenameFromPath texMap.filename
	
	format "\t\t <texture type=\"%\" filename=\"%\"/>\n" texTypeName (getTexturePath textureName) to:file
	
	local ext = toLower (getFilenameType textureName)
	if (findItem acceptedExtensions ext) == 0 then
	(
		messageBox ("La textura " + texturePath +" no tiene formato aceptable!") title:"MENSAJE PARA ARTISTAS"
	)
	else if shouldCopyTextures then
	(
		format "Copying % from % to output dir %\n" texTypeName texturePath (getAbsTexturePath textureName)
		copyFile texturePath (getAbsTexturePath textureName)
	)
)

fn exportSingleTexture mat meshName = (
	if HasTexture mat alertOnMissing:true then
	(
		exportTextureFile "albedo" mat.diffuseMap
	)
)

fn exportSingleReflection mat meshName = (
	if HasReflection mat alertOnMissing:false then
	(
		exportTextureFile "environment" mat.reflectionMap
	)
)

fn exportSingleLightmap mat meshName = (
	if HasLightmap mat alertOnMissing:false then
	(
		exportTextureFile "lightmap" mat.selfIllumMap
	)
)

fn exportSingleNormal mat meshName = (
	if HasNormal mat alertOnMissing:false then
	(
		exportTextureFile "normal" mat.bumpMap
	)
)

fn createSingleMaterial obj mat = (
	
	-- TODO cambiar technique dependiendo del tipo de material
	
	local vtype = GetVertexTypeStringFromObjectMaterial mat
		
	
	local ssr = 0
	local environment = 0
	
	if mat != undefined then
	(
		local reflectionFactor = mat.reflectionMapAmount / 100.0
		local reflect = mat.reflectionMapEnable
		
		if reflect then
		(
			if mat.reflectionMap != undefined and classof mat.reflectionMap == Bitmaptexture and mat.reflectionMap.bitmap != undefined then
			(
				environment = reflectionFactor
				vtype = vtype + "_ENVIRONMENT"
			)
			else
			(
				ssr = reflectionFactor
			)
		)
	)
	
	
	
	format "\t<material name=\"%\" vertex_type=\"%\" >\n" mat.name (vtype) to:file	

	exportSingleTexture mat obj.name
	
	exportSingleReflection mat obj.name
	
	exportSingleLightmap mat obj.name
	
	exportSingleNormal mat obj.name
	
	format "\t\t<parameter type=\"float\" name=\"specular\" value=\"0.05\" varParams=\"min=0 max=1 step=0.01 precision=2\"/>\n" to:file
	format "\t\t<parameter type=\"float\" name=\"gloss\" value=\"80.0\" varParams=\"min=1 max=100 step=0.5 precision=1\"/>\n" to:file
	format "\t\t<parameter type=\"float\" name=\"bump\" value=\"2.4\" varParams=\"min=0 max=5 step=0.1 precision=1\"/>\n" to:file
	format "\t\t<parameter type=\"float\" name=\"environment_factor\" value=\"%\" varParams=\"min=0 max=1 step=0.01 precision=2\"/>\n" environment to:file
	format "\t\t<parameter type=\"float\" name=\"cutout_factor\" value=\"1.0\" varParams=\"min=0 max=1 step=0.01 precision=2\"/>\n" to:file
	format "\t\t<parameter type=\"float\" name=\"ssr_factor\" value=\"%\" varParams=\"min=0 max=1 step=0.01 precision=2\"/>\n" ssr to:file
	format "\t\t<parameter type=\"Vect2f\" name=\"tex_velocity\" value=\"0 0\" varParams=\"min=0 max=1 step=0.01 precision=2\"/>\n" to:file
	format "\t\t<parameter type=\"float\" name=\"receive_shadows\" value=\"1\" varParams=\"min=0 max=1 step=1 precision=0\"/>\n" to:file
	
	format "\t</material>\n" to:file 
)

fn matExistsElseAdd matMap mat = (
	if matMap.ContainsKey (mat.name) then
	(
		return true
	)
	else
	(
		matMap.Add mat.name 0
		return false
	)
)

fn createMaterials sceneName copyTextures = (
	MyCreateFile ( MaterialsSubPath + sceneName + ".xml" )
	
	local cores = GetCores()
	
	WriteXMLPrologToFile()
	format "<materials>\n" to:file
	
	local l_MatsMap = dotnetobject "System.Collections.Generic.Dictionary`2[System.String,System.Int16]"
	
	shouldCopyTextures = copyTextures
	
	for i=1 to cores.count do
	(
		print cores[i].name
		if classof cores[i].material == Standardmaterial then
		(
			print cores[i].material.name
			if not matExistsElseAdd l_MatsMap cores[i].material then
			(
				createSingleMaterial cores[i] cores[i].material
			)
		)
		else if cores[i].material != undefined then -- Si se trata de un multimaterial
		(
			for j = 1 to cores[i].material.numsubs do
			(		
				--Si ya existe no exportamos
				if not matExistsElseAdd l_MatsMap cores[i].material[j] then
				(
					createSingleMaterial cores[i] cores[i].material[j]
				)
			)
		)
	)
	
	format "</materials>\n" to:file 
	format "____CreatedMaterials____\n"
	
	MyCloseFile()
)
