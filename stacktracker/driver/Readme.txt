This directory contains the driver used by the Arm EBC stack tracker test suite.

An Arm toolchain as well as the gnu-efi git submodule are required for compilation.

Note that, to enable Arm compilation from Visual Studio 2015, you should:
- Make sure Visual Studio is fully closed.
- Navigate to C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0\V140\Platforms\ARM
  and remove the read-only attribute on Platform.Common.props.
- Using a text editor running with Administrative privileges open:
  C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0\V140\Platforms\ARM\Platform.Common.props
- Under the <PropertyGroup> section add the following:
  <WindowsSDKDesktopARMSupport>true</WindowsSDKDesktopARMSupport>
