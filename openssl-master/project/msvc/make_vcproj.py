import shutil
import os
import sys
import xml.dom.minidom

def strip_x64_target(inproj, outproj):
  # load the config xml file
  projIn = xml.dom.minidom.parse(inproj)
  
  platforms = projIn.getElementsByTagName("Platforms")[0]
  elements = projIn.getElementsByTagName("Platform")
  
  for element in elements :
    print element.getAttribute("Name")
    platform = element.getAttribute("Name")
    if platform == "x64" :
      print "removing x64"         
      platforms.removeChild(element)
  
  configuration = projIn.getElementsByTagName("Configurations")[0]
  elements = projIn.getElementsByTagName("Configuration")
  
  for element in elements :
    print element.getAttribute("Name")
    platform = element.getAttribute("Name")
    if platform == "Debug|x64" or platform == "Release|x64" or platform == "Debug_CU|x64" or platform == "Release_CU|x64":
      print platform         
      configuration.removeChild(element)
    
  f = open(outproj,'w')
  f.write( projIn.toprettyxml() )  
  f.close()

def update_platform_toolset(inproj, outproj):
  projIn = xml.dom.minidom.parse(inproj)

  elements = projIn.getElementsByTagName("PropertyGroup")

  for element in elements :
    print element.getAttribute("Label")
    label = element.getAttribute("Label")
    if label == "Configuration" :
      print "adding toolset"
      toolset = projIn.createElement("PlatformToolset")
      value = projIn.createTextNode("v110")
      toolset.appendChild(value)
      element.appendChild(toolset)
    
  f = open(outproj,'w')
  f.write( projIn.toprettyxml() )  
  f.close()

if __name__ == '__main__':
  projbasename = sys.argv[1]
  print "Project : " + projbasename
  
  #
  # Convert the unnamed pantheon (2008) project to named 2008 project.
  #
  
  shutil.copy(projbasename+'.vcproj', projbasename+'.2008.vcproj')
        
  #
  # Create the 2012 project from the 2008 project. (must not update from vs2010 as it set incorrect toolset)
  #
  
  if os.path.exists(projbasename+'.2012.vcxproj'):
  	os.remove(projbasename+'.2012.vcxproj')
  	
  if os.path.exists(projbasename+'.2012.vcxproj.filters'):
  	os.remove(projbasename+'.2012.vcxproj.filters')

  shutil.copy(projbasename+'.2008.vcproj', projbasename+'.2012.vcproj')
  os.system('config_visual_studio_2012 & vcupgrade ' + projbasename+'.2012.vcproj')
  
  #
  # Create the 2010 project from the 2008 project.
  #
  
  if os.path.exists(projbasename+'.2010.vcproj'):
  	os.remove(projbasename+'.2010.vcproj')
  
  if os.path.exists(projbasename+'.2010.vcxproj'):
  	os.remove(projbasename+'.2010.vcxproj')
  	
  if os.path.exists(projbasename+'.2010.vcxproj.filters'):
  	os.remove(projbasename+'.2010.vcxproj.filters')

  strip_x64_target(projbasename+'.2008.vcproj', projbasename+'.2010.vcproj')
  os.system('config_visual_studio_2010_express & vcupgrade ' + projbasename+'.2010.vcproj')
  
  #
  # Fix props
  #
  
  props = open('vsprops_local\common.props', 'r')
  data = props.read().replace(r'ConfigurationName', r'Configuration')
  props.close()
  
  props = open('vsprops_local\common.props', 'w')
  props.write(data)
  props.close()

  #
  # clean up
  #
  
  if os.path.exists(projbasename+'.2010.vcproj'):
  	os.remove(projbasename+'.2010.vcproj')
  
  if os.path.exists(projbasename+'.2012.vcproj'):
  	os.remove(projbasename+'.2012.vcproj')
