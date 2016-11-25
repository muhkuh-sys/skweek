require("muhkuh_cli_init")

if #arg~=1 then
  error("Missing parameter: tune to play.")
end
strFileName = arg[1]

-- Load the tune.
tFile = io.open(strFileName, "r")
if tFile==nil then
  error(string.format("Failed to open file '%s'!", strFileName))
end
strTune = tFile:read("*a")
tFile:close()
sizTune = string.len(strTune)

local tPlugin = tester.getCommonPlugin()
if tPlugin==nil then
	error("No plug-in selected, nothing to do!")
end

strBinaryName = "netx/skweek_netx4000.bin"

-- Download and run the binary.
local aAttr = tester.mbin_open(strBinaryName)
tester.mbin_debug(aAttr)
tester.mbin_write(nil, tPlugin, aAttr)

-- Get the buffer for the tune.
-- NOTE: the first 32 bytes are reserved for the test parameter headers.
pucBuffer = aAttr.ulParameterStartAddress + 32
sizBuffer = aAttr.ulParameterEndAddress - pucBuffer

-- Does the tune fit into the buffer?
if sizTune>sizBuffer then
  error(string.format("The size of tune (%d bytes) exceeds the buffer size (%d bytes).", sizTune, sizBuffer))
end

-- Download the tune.
print("Downloading the tune...")
tPlugin:write_image(pucBuffer, strTune, tester.callback_progress, sizTune)

aulParameter = { aAttr.ulParameterStartAddress, sizTune }
tester.mbin_set_parameter(tPlugin, aAttr, aulParameter)
local ulResult = tester.mbin_execute(nil, tPlugin, aAttr)
if ulResult~=0 then
	error(string.format("The CRC test returned an error code: 0x%08x", ulResult))
end

