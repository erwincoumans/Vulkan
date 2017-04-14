import glob
import os
import shutil
import subprocess  
import sys
  
from distutils.dir_util import copy_tree

####################
#
# Functions
#
####################

def copy(src, dst):
    # Create directory if needed
    if not os.path.exists(dst):
        os.makedirs(dst)

    # Copy exact file or given by wildcard 
    for filename in glob.glob(src):
        print("Copying asset '%s'" % (os.path.basename(filename)))
        shutil.copy(filename, dst)        
        
def validate():
        
    os.chdir("libs")    
        
    allArchs = os.listdir()

    for arch in allArchs:
        copy_tree(os.environ['ANDROID_NDK_HOME'] + "/sources/third_party/vulkan/src/build-android/jniLibs/" + arch, arch)
    
    os.chdir("..")
        
####################
#
# Main
#
####################

print("Copying project assets")

copy("../../VKTS_Binaries/shader/SPIR/V/font.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/font.frag.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/font_df.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/font_df.frag.spv", "./assets/shader/SPIR/V/")

copy("../../VKTS_Binaries/shader/SPIR/V/bsdf.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf_no_texcoord.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf_tangents.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf_skinning.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf_tangents_two_texcoord.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/bsdf_skinning_two_texcoord.vert.spv", "./assets/shader/SPIR/V/")

copy("../../VKTS_Binaries/shader/SPIR/V/glTF*.spv", "./assets/shader/SPIR/V/")

copy("../../VKTS_Binaries/shader/SPIR/V/environment.vert.spv", "./assets/shader/SPIR/V/")
copy("../../VKTS_Binaries/shader/SPIR/V/environment.frag.spv", "./assets/shader/SPIR/V/")

copy("../../VKTS_Binaries/font/*.fnt", "./assets/font/")
copy("../../VKTS_Binaries/font/*.tga", "./assets/font/")

copy("../../VKTS_Binaries/glTF/mr/Corset/*.*", "./assets/glTF/mr/Corset/")

copy("../../VKTS_Binaries/environment/road*.*", "./assets/environment/")

copy("../../VKTS_Binaries/primitives/*.vkts", "./assets/primitives/")

copy("../../VKTS_Binaries/texture/BSDF_LUT_512_256.data", "./assets/texture/")

copy("../../VKTS_Binaries/cache/environment/road*.hdr", "./assets/cache/environment/")


print("Building project")

os.chdir("jni")

subprocess.call("ndk-build -j", shell=True)

os.chdir("..")

for x in range(1, len(sys.argv)):
    if sys.argv[x] == "validate":
        validate()
        break
        
subprocess.call("ant debug", shell=True)  
