erase %~dp0\vsprops_local\debug.props
erase %~dp0\vsprops_local\common.props
python make_vcproj.py openssl
pause