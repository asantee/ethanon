/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "EditorBase.h"

#include "../engine/ETHTypes.h"
#include "../engine/Util/ETHASUtil.h"

bool DirectoryExists(const std::string& dir)
{
	#warning todo
	return false;
}

static NSArray* GenerateFileTypesArray(const FILE_FORM_FILTER& filter)
{
	NSMutableArray* r = [NSMutableArray new];
	for (std::size_t t = 0; t < filter.extensionsAllowed.size(); t++)
	{
		[r addObject:[NSString stringWithUTF8String:filter.extensionsAllowed[t].c_str()]];
	}
	return r;
}

static bool OpenPanel(
	NSSavePanel* panel,
	const FILE_FORM_FILTER& filter,
	const char *directory,
	char *szoFilePathName,
	char *szoFileName)
{
	NSArray* fileTypes = GenerateFileTypesArray(filter);

	[panel setFloatingPanel:YES];
	[panel setAllowedFileTypes:fileTypes];
	[panel setMessage:[NSString stringWithUTF8String:filter.title.c_str()]];

	std::string initDirectory = "file://localhost";
	initDirectory.append(directory);
	NSURL* initPath = [NSURL URLWithString:[NSString stringWithUTF8String:initDirectory.c_str()]];
	[panel setDirectoryURL:initPath];

	const long i = [panel runModal];
	const bool r = (i == NSFileHandlingPanelOKButton);

	if (r)
	{
		NSURL* fileNameURL = [panel URL];
		NSString* fileNameString = [fileNameURL path];
		const std::string fileName = [fileNameString cStringUsingEncoding:1];
		strcpy(szoFilePathName, fileName.c_str());
		strcpy(szoFileName,     Platform::GetFileName(fileName).c_str());
	}
	return r;

}

bool EditorBase::OpenForm(
	const FILE_FORM_FILTER& filter,
	const char* directory,
	char* szoFilePathName,
	char* szoFileName)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSOpenPanel *panel;
	panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setCanChooseFiles:YES];
	[panel setAllowsMultipleSelection:NO];

	const bool r = OpenPanel(panel, filter, directory, szoFilePathName, szoFileName);
	[pool release];
	return r;
}

bool EditorBase::SaveForm(
	const FILE_FORM_FILTER& filter,
	const char* directory,
	char* szoFilePathName,
	char* szoFileName)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSSavePanel *panel;
	panel = [NSSavePanel savePanel];

	const bool r = OpenPanel(panel, filter, directory, szoFilePathName, szoFileName);
	[pool release];
	return r;
}
