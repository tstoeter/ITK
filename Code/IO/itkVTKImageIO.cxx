/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVTKImageIO.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "itkVTKImageIO.h"

namespace itk
{

VTKImageIO::VTKImageIO()
{
  this->SetNumberOfDimensions(2);
  m_ByteOrder = LittleEndian;
  m_FileType = ASCII;
  
}

VTKImageIO::~VTKImageIO()
{
}

bool VTKImageIO::OpenVTKFile(std::ofstream& os, const char* filename,
                             int openMode)
{
  // Make sure that we have a file to 
  if ( filename == "" )
    {
    itkErrorMacro(<<"A FileName must be specified.");
    return false;
    }

  // Close file from any previous image
  if ( os.is_open() )
    {
    os.close();
    }
  
  // Open the new file for reading
  itkDebugMacro(<< "Initialize: opening file " << filename);

  // Actually open the file
#ifdef _WIN32
  openMode |= std::ios::binary;
#endif
  os.open(filename, openMode);
  if ( os.fail() )
    {
    itkErrorMacro(<< "Could not open file: " << filename);
    return false;
    }

  return true;
}


bool VTKImageIO::CanReadFile(const char* filename) 
{ 
  std::ofstream file;
  std::string fname(filename);

  if ( ! this->OpenVTKFile(file,m_FileName.c_str(),std::ios::in) )
    {
    return false;
    }

  // Check to see if its a vtk structured points file
  if ( fname.find(".vtk") < fname.length() )
    {
    }
  
  return true;
}
  
 
void VTKImageIO::Read(void* buffer)
{
  // Read the header, make sure it is

}

void VTKImageIO::ReadImageInformation()
{
}

bool VTKImageIO::CanWriteFile(const char*)
{
  if ( m_FileName != "" &&
       m_FileName.find(".vtk") < m_FileName.length() )
    {
    return true;
    }
  return false;
}

void VTKImageIO::Write(void* buffer)
{
  std::ofstream file;
  if ( ! this->OpenVTKFile(file,m_FileName.c_str(),std::ios::out) )
    {
    return;
    }

  // Check the image region for proper dimensions, etc.
  unsigned int numDims = this->GetNumberOfDimensions();
  if ( numDims < 2 || numDims > 3 )
    {
    itkErrorMacro(<<"VTK Writer can only write 2 or 3-dimensional images");
    return;
    }
  ImageIORegion ioRegion = this->GetIORegion();

  // Write the VTK header information
  file << "# vtk DataFile Version 3.0\n"; 
  file << "VTK File Generated by Insight Segmentation and Registration Toolkit (ITK)\n";

  if ( this->GetFileType() == ASCII ) { file << "ASCII\n"; }
  else { file << "BINARY\n"; }

  // Write characteristics of the data
  file << "DATASET STRUCTURED_POINTS\n";
  if ( numDims == 2 )
    {
    file << "DIMENSIONS " << this->GetDimensions(0) << " "
           << this->GetDimensions(1) << " 1\n";
    file << "SPACING " << m_Spacing[0] << " " << m_Spacing[1] << " 1.0\n";
    file << "ORIGIN " << m_Origin[0] << " " << m_Origin[1] << " 0.0\n";
    }
  else //numDims == 3
    {
    file << "DIMENSIONS " << this->GetDimensions(0) << " "
           << this->GetDimensions(1) << this->GetDimensions(2) << "\n";
    file << "SPACING " << m_Spacing[0] << " " 
           << m_Spacing[1] << " " << m_Spacing[2] << "\n";
    file << "ORIGIN " << m_Origin[0] << " "
           << m_Origin[1] << " " << m_Origin[2] << "\n";
    }

  file << "POINT_DATA " << this->GetImageSizeInPixels() << "\n";
  file << "SCALARS scalars " 
         << this->ReturnTypeAsString(this->GetComponentType()) << " "
         << this->GetNumberOfComponents() << "\n";
  file << "LOOKUP_TABLE default\n";

  // Write the actual pixel data
  if ( m_FileType == ASCII )
    {
    this->WriteBufferAsASCII(file, buffer, this->GetComponentType(),
                             this->GetImageSizeInComponents());
    }
  else //binary
    {
    file.write(static_cast<char*>(buffer), this->GetImageSizeInBytes());
    }
}

void VTKImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace itk
