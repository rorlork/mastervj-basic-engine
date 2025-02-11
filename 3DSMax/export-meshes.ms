fileIn "file-utils.ms"
fileIn "vertex-types.ms"
fileIn "export-scene-utils.ms"

fn getRealFaceMatID obj faceId = (
	local numMats = 1
	if (classOf obj.material) != Standardmaterial then
	(
		numMats = obj.material.numsubs
	)
	local faceMatId = getFaceMatID obj faceId
	faceMatId = int(mod (faceMatId-1) numMats)+1
	return faceMatId
)

fn getMaterial obj matId = (	
	if (classOf obj.material) == Standardmaterial then
	(
		return obj.material
	)
	else
	(
		return obj.material[matId]
	)
)

fn getUsedMaterials obj = (
	
	if (obj.material == undefined) then
	(
		return #()
	)
	
	if (classOf obj.material) == Standardmaterial then
	(
		return #(1)
	)
	
	local maxNumMats = obj.material.numsubs
	local facesPerMat = #()
	for i = 1 to maxNumMats do
	(
		append facesPerMat 0
	)
	
	local NumFaces=getNumFaces obj

	for IdFace=1 to NumFaces do
	(
		local faceMatId = getRealFaceMatID obj IdFace
		facesPerMat[faceMatId] = facesPerMat[faceMatId]+1
	)
	local matsUsed = #()
	
	for i = 1 to maxNumMats do
	(
		if facesPerMat[i] > 0 then
		(
			append matsUsed i
		)
	)
	
	return matsUsed
)


fn createStaticMeshesFile sceneName meshesToExport = (
	MyCreateFile ("static_meshes_" + sceneName + ".xml")
	
	local cores = meshesToExport

	WriteXMLPrologToFile()
	format "<static_meshes>\n" to:file
	
	for i=1 to cores.count do
	(
		
		-- Clone mesh and use the clone
		if superclassof cores[i] != GeometryClass then
		(
			format "$$$$ % no es una malla!\n" cores[i].name
			continue
		)
		local clone = #()
		maxOps.cloneNodes cores[i] newNodes:&clone
		if clone.count > 1 then
		(
			delete clone
			format "$$$$ % no es un objeto adecuado!\n" cores[i]
			continue
		)
		local nn = getUserProp cores[i] "export_core_name"
		local n = cores[i].name
		cores[i].name = "_" + n
		clone[1].name = n
		
		format "\t<static_mesh name=\"%\" filename=\"%.mesh\">\n" nn (getMeshPath nn) to:file
		local mats = getUsedMaterials(convertToMesh clone[1])
		for j = 1 to mats.count do
		(
			format "\t\t<material name=\"%\" />\n" (getMaterial clone[1] mats[j]).name to:file
		)
		format "\t</static_mesh>\n\n" to:file
		
		delete clone
		cores[i].name = n
	)
	
	format "</static_meshes>\n" to:file 
	format "____CreatedStatic____\n"
	
	MyCloseFile()
)


---------------------------------------------------------------------
-- NORMALS
---------------------------------------------------------------------
global UABVtxsNormals=#()
function UABClearNormalsArray =
(
	while UABVtxsNormals.count>0 do
	(
		deleteItem UABVtxsNormals 1
	)
)

function IsSmoothingGroupEnabled IdGroup Value =
(
	local ValueMask=2^(IdGroup-1)
	return (bit.and Value ValueMask)==ValueMask
)
function UABGetVertexNormal obj IdVertex SmoothValue FaceNormal =
(
	local HasNormal=false
	local Normal=point3 0 0 0
	for b=1 to 32 do
	(
		if((IsSmoothingGroupEnabled b SmoothValue)==true) then
		(
			Normal=Normal+UABVtxsNormals[IdVertex][b]
			HasNormal=true
		)
	)
	if HasNormal==false then
	(
		Normal=FaceNormal
	)
	return Normal
)
function UABCalcVertexsNormals obj =
(
	UABClearNormalsArray()
	local NumVtxs=getNumVerts obj
	for b=1 to NumVtxs do
	(
		UABVtxsNormals[b]=#()
		for t=1 to 32 do
		(
			UABVtxsNormals[b][t]=point3 0 0 0
		)
	)
	local NumFaces=getNumFaces obj
	local InvTransform=inverse obj.transform
	for IdFace=1 to NumFaces do
	(
		local IdxsFace=getFace obj IdFace
		local Vtx1=(getVert obj IdxsFace.x)*InvTransform
		local Vtx2=(getVert obj IdxsFace.y)*InvTransform
		local Vtx3=(getVert obj IdxsFace.z)*InvTransform
		local FaceNormal=getFaceNormal obj IdFace
		local SmoothValue=getFaceSmoothGroup obj IdFace
		for b=1 to 32 do
		(
			if((IsSmoothingGroupEnabled b SmoothValue)==true) then
			(
				UABVtxsNormals[IdxsFace.x][b]=UABVtxsNormals[IdxsFace.x][b] + FaceNormal
				UABVtxsNormals[IdxsFace.y][b]=UABVtxsNormals[IdxsFace.y][b] + FaceNormal
				UABVtxsNormals[IdxsFace.z][b]=UABVtxsNormals[IdxsFace.z][b] + FaceNormal
			)
		)
	)
)

---------------------------------------------------------------------
-- /NORMALS
---------------------------------------------------------------------



function ConvertVtx Vtx =
(
	local l_Text = stringstream ""
	format "%" Vtx to:l_Text
	return (l_Text as string)
)

function DotNetAddVertex VtxArray VtxMap Vtx =
(
	local l_VtxString=ConvertVtx Vtx
	local l_Idx=1
	if (VtxMap.ContainsKey (l_VtxString))==false then
	(
		l_Idx=VtxArray.count
		append VtxArray Vtx
		VtxMap.Add l_VtxString l_Idx
	)
	else
		l_Idx=VtxMap.Item[l_VtxString]
	return l_Idx
)

global CurrentObjInvMat = matrix3 1
global CurrentNrmInvMat = matrix3 1

fn createVertexesForFace obj IdFace matDesc = (
	local IdxsFace=getFace obj IdFace
	-- Para coger los v?rtices de la primera cara
	local Vtx1=getVert obj IdxsFace[1]
	local Vtx2=getVert obj IdxsFace[2]
	local Vtx3=getVert obj IdxsFace[3]
	
	-- TODO: Obtener normales correctas
	
	-- Para coger las normales de los v?rtices de la primera cara
	local FaceNormal = getFaceNormal obj IdFace -- El ?ndice de la cara
	local SmoothValue = getFaceSmoothGroup obj IdFace -- El ?ndice de la cara

	--local Normal1=getNormal obj IdxsFace[1]
	--local Normal2=getNormal obj IdxsFace[2]
	--local Normal3=getNormal obj IdxsFace[3]
	local Normal1=(normalize (UABGetVertexNormal obj IdxsFace[1] SmoothValue FaceNormal))*CurrentNrmInvMat
	local Normal2=(normalize (UABGetVertexNormal obj IdxsFace[2] SmoothValue FaceNormal))*CurrentNrmInvMat
	local Normal3=(normalize (UABGetVertexNormal obj IdxsFace[3] SmoothValue FaceNormal))*CurrentNrmInvMat
	
	
	local v1 = vertex pos: (ChangeCoordsBase (Vtx1*CurrentObjInvMat) ) normal: (ChangeCoordsBase Normal1) uv: (point3 0 0 0) uv2: (point3 0 0 0) binormal: [1,0,0] tangencial: [0,0,1]
	local v2 = vertex pos: (ChangeCoordsBase (Vtx2*CurrentObjInvMat) ) normal: (ChangeCoordsBase Normal2) uv: (point3 0 0 0) uv2: (point3 0 0 0) binormal: [1,0,0] tangencial: [0,0,1]
	local v3 = vertex pos: (ChangeCoordsBase (Vtx3*CurrentObjInvMat) ) normal: (ChangeCoordsBase Normal3) uv: (point3 0 0 0) uv2: (point3 0 0 0) binormal: [1,0,0] tangencial: [0,0,1]
	
	if matDesc.texture then -- map[0] is the color, map[1] is the default texture, so at least 1 map (color) is always there
	(
		-- Para coger los ?ndices de las coordenadas de textura de la primera cara, el 1 se?ala al primer
		--   conjunto de coordenadas aplicadas sobre la cara
		local IdxsMap=meshop.getMapFace obj 1 IdFace
		
		-- Para coger las coordenadas de los v?rtices de la primera cara
		local TUVMap1= meshop.getMapVert obj 1 IdxsMap[1]
		local TUVMap2= meshop.getMapVert obj 1 IdxsMap[2]
		local TUVMap3= meshop.getMapVert obj 1 IdxsMap[3]
		
		v1.uv = TUVMap1
		v2.uv = TUVMap2
		v3.uv = TUVMap3
		
	)
	
	if matDesc.environment_map then -- map[0] is the color, map[1] is the default texture, so at least 1 map (color) is always there
	(
		-- Para coger los ?ndices de las coordenadas de textura de la primera cara, el 1 se?ala al primer
		--   conjunto de coordenadas aplicadas sobre la cara
		local IdxsMap=meshop.getMapFace obj 1 IdFace
		
		-- Para coger las coordenadas de los v?rtices de la primera cara
		local TUVMap1= meshop.getMapVert obj 1 IdxsMap[1]
		local TUVMap2= meshop.getMapVert obj 1 IdxsMap[2]
		local TUVMap3= meshop.getMapVert obj 1 IdxsMap[3]
		
		v1.uv = TUVMap1
		v2.uv = TUVMap2
		v3.uv = TUVMap3
		
	)
	
	if matDesc.light_map then
	(
		local IdxsMap2=meshop.getMapFace obj 2 IdFace
		
		-- Coordenades de lightmap
		local LUVMap1= meshop.getMapVert obj 2 IdxsMap2[1]
		local LUVMap2= meshop.getMapVert obj 2 IdxsMap2[2]
		local LUVMap3= meshop.getMapVert obj 2 IdxsMap2[3]
		
		v1.uv2 = LUVMap1
		v2.uv2 = LUVMap2
		v3.uv2 = LUVMap3
	)
	
	if matDesc.normal_map then
	(		
		--Binormal & Tang x Face
		--Font: http://forums.cgsociety.org/archive/index.php?t-407814.html
		local dV1 = Vtx1 - Vtx2
		local dV2 = Vtx1 - Vtx3

		local dUV1 = TUVMap1 - TUVMap2
		local dUV2 = TUVMap1 - TUVMap3

		local area = dUV1.x * dUV2.y - dUV1.y * dUV2.x
		local sign = if area < 0 then -1 else 1

		v1.tangencial = [0,0,1]

		v1.tangencial.x = dV1.x * dUV2.y - dUV1.y * dV2.x
		v1.tangencial.y = dV1.y * dUV2.y - dUV1.y * dV2.y
		v1.tangencial.z = dV1.z * dUV2.y - dUV1.y * dV2.z

		v1.tangencial = (normalize v1.tangencial) * sign

		v1.binormal = (normalize (cross Normal1 v1.tangencial)) * sign

		v2.tangencial = v1.tangencial
		v3.tangencial = v1.tangencial
		v2.binormal = v1.binormal
		v3.binormal = v1.binormal		
	)
	
	return #(v1, v2, v3)

)

fn writeMeshToFile file matDescriptor vtxs idxs = (
	
	if vtxs.count > 65535 then
	(
		format "\n$$$$ Object % has too many vertices: %. Maximum 65535.\n\n" obj.name vtxs.count
		messageBox ("Object "+obj.name+" has too many vertices: " + (vtxs.count as string) + ". Maximum 65535.") title: "Error!"
		MyCloseFile()
		return false
	)
	if idxs.count > 65535 then
	(
		format "\n$$$$ Object % has too many faces: %. Maximum 65535.\n\n" obj.name vtxs.count
		messageBox ("Object "+obj.name+" has too many faces: " + (vtxs.count as string) + ". Maximum 65535.") title: "Error!"
		MyCloseFile()
		return false
	)
	
	writeshort file vtxs.count
	
	tik()
	for idVtx = 1 to vtxs.count do
	(
		local vtx = vtxs[idVtx]
		-- Vertex
		writefloat file vtx.pos.x
		writefloat file vtx.pos.y
		writefloat file vtx.pos.z
		
		-- Normal
		writefloat file vtx.normal.x
		writefloat file vtx.normal.y
		writefloat file vtx.normal.z

		if matDescriptor.normal_map then
		(
			-- Binormal
			writefloat file vtx.binormal.x
			writefloat file vtx.binormal.y
			writefloat file vtx.binormal.z
			writefloat file 0
			-- Tangencial
			writefloat file vtx.tangencial.x
			writefloat file vtx.tangencial.y
			writefloat file vtx.tangencial.z
			writefloat file 0
		)
		
		if matDescriptor.texture or matDescriptor.environment_map then
		(
			-- UV
			writefloat file vtx.uv.x
			writefloat file (1 - vtx.uv.y)
		)
		
		if matDescriptor.light_map then
		(
			--UV Lightmap
			writefloat file vtx.uv2.x
			writefloat file (1 - vtx.uv2.y)
		)
	)
	tok s:"write all vertices for this material"
	
	-- Write the index byte width
	writeshort file 16
	
	writeshort file idxs.count
	
	tik()
	for idx = 1 to idxs.count do
	(
		writeshort file idxs[idx]
	)
	tok s:"write indices for this material's vertices"
	
	return true
)

fn createMesh obj coreName = (
	
	local l_coreName = coreName
	if (coreName == undefined) then
	(
		l_coreName = obj.name
	)
	
	format "____Creating Core % for Mesh %____\n" l_coreName obj.name
	
	MyFOpen (MeshSubPath + l_coreName + ".mesh") "wb"

	format "  Open file %\n" (MeshSubPath + l_coreName + ".mesh")
	
	-- Centrar al origen
	--obj.position = [0, 0, 0]
	--obj.rotation = quat 0 0 0 1
	--obj.scale = [1, 1, 1]
	
	CurrentObjInvMat = inverse obj.transform
	CurrentNrmInvMat = inverse (obj.transform.rotation as matrix3)
	
	tik()
	UABCalcVertexsNormals obj
	tok s:"Calc Vertex Normals"
	
	--Header
	writeshort file 0xFE56
	local numMats = 1
	
	if obj.material == undefined then
	(
		format "\n$$$$ Object % doesn't have a material!\n\n" obj.name
		--messageBox ("Object "+obj.name+" doesn't have a material!") title: "Error!"
		--MyCloseFile()
		--return OK
	)
	
	--Materials
	local usedMaterials = getUsedMaterials obj
	
	if usedMaterials.count > 0  then
	(
		writeshort file usedMaterials.count
	)
	else
	(
		format "Exporting mesh without material\n"
		numMats = 1
		writeshort file numMats
		
		local l_VtxsMap = dotnetobject "System.Collections.Generic.Dictionary`2[System.String,System.Int16]"
		local idxs = #()
		local vtxs = #()
		
		writeshort file (GetVertexTypeFromObjectMaterial obj undefined)
		
		tik()
		local matDescriptor = GetMatDescriptor undefined
		
		for idFace = 1 to (getNumFaces obj) do
		(
			--format "idFace %\n" idFace
			local faceVtxs = createVertexesForFace obj idFace matDescriptor
			for numVtx = 1 to 3 do
			(
				local idx = DotNetAddVertex vtxs l_VtxsMap faceVtxs[numVtx]
				append idxs idx
			)
		)
		tok s:"gather all vertices for this mesh"
		
		local r = writeMeshToFile file matDescriptor vtxs idxs
		if not r then
		(
			MyCloseFile()
			return Ok
		)
	)
	
	--Vertex
	
	for i=1 to usedMaterials.count do
	(
		local l_VtxsMap = dotnetobject "System.Collections.Generic.Dictionary`2[System.String,System.Int16]"
		local idxs = #()
		local vtxs = #()
		
		local mat = getMaterial obj usedMaterials[i]
		
		writeshort file (GetVertexTypeFromObjectMaterial obj mat)
		
		tik()
		local matDescriptor = GetMatDescriptor mat
		
		for idFace = 1 to (getNumFaces obj) do
		(
			--format "idFace %\n" idFace
			local faceMatId = getRealFaceMatID obj IdFace
			-- coger solo vertices afectados por ese mat...
			if faceMatId == usedMaterials[i] then
			(
				local faceVtxs = createVertexesForFace obj idFace matDescriptor
				for numVtx = 1 to 3 do
				(
					local idx = DotNetAddVertex vtxs l_VtxsMap faceVtxs[numVtx]
					append idxs idx
				)
			)
		)
		tok s:"gather all vertices for this material"
		
		local r = writeMeshToFile file matDescriptor vtxs idxs
		if not r then
		(
			MyCloseFile()
			return Ok
		)
	)
	
	-- TODO: Export bounding box/sphere? look into NodeGetBoundingBox
	
	--Footer
	writeshort file 0x55FE
	
	MyCloseFile()
	
	format "____Created Mesh %____\n" obj.name
)



fn createMeshes meshesToExport = (
	local cores = meshesToExport
	for i=1 to cores.count do
	(
		-- Clone mesh and use the clone
		if superclassof cores[i] != GeometryClass then
		(
			format "$$$$ % no es una malla!\n" cores[i].name
			continue
		)
		local clone = #()
		maxOps.cloneNodes cores[i] newNodes:&clone
		if clone.count > 1 then
		(
			delete clone
			format "$$$$ % no es un objeto adecuado!\n" cores[i]
			continue
		)
		local n = cores[i].name
		cores[i].name = "_" + n
		clone[1].name = n
		
		local l_coreName = getUserProp clone[1] "export_core_name"
		
		createMesh (convertToMesh clone[1]) l_coreName
		
		delete clone
		cores[i].name = n
	)
)
