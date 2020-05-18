::删除编译生成的文件
rd /s /q .\nRF5_SDK_15.3.0_59ac345\examples\ble_peripheral\ble_app_uart\pca10040\s132\arm5_no_packs\_build
::del /f /s /q .\nRF5_SDK_15.3.0_59ac345\examples\ble_peripheral\ble_app_uart\pca10040\s132\arm5_no_packs\hex\softdeviceAndBootLoader.hex

::上传到GitHub，非开发者无使用权限，可屏蔽
git add .
git commit
git push

::pause