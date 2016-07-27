require("muhkuh_cli_init")

local tPlugin = tester.getCommonPlugin()
if tPlugin==nil then
	error("No plug-in selected, nothing to do!")
end

strBinaryName = "netx/skweek_netx4000.bin"

-- Download and run the binary.
local aAttr = tester.mbin_open(strBinaryName)
tester.mbin_debug(aAttr)
tester.mbin_write(nil, tPlugin, aAttr)
tester.mbin_set_parameter(tPlugin, aAttr, aulParameter)
local ulResult = tester.mbin_execute(nil, tPlugin, aAttr)
if ulResult~=0 then
	error(string.format("The CRC test returned an error code: 0x%08x", ulResult))
end

