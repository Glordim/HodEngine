#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#import <Cocoa/Cocoa.h>

namespace hod::window
{
	/// @brief 
	/// @return 
	Path GetOpenFileDialog()
	{
		NSOpenPanel *panel = [NSOpenPanel openPanel];
    
		// Configurer la boîte de dialogue
		[panel setCanChooseFiles:NO]; // Ne permet pas la sélection de fichiers
		[panel setCanChooseDirectories:YES]; // Permet la sélection de dossiers
		[panel setAllowsMultipleSelection:NO]; // Ne permet pas la sélection multiple
		[panel setTitle:@"Select a folder"];
		
		// Afficher la boîte de dialogue et gérer la sélection
		if ([panel runModal] == NSModalResponseOK) {
			NSURL *selectedDirectoryURL = [[panel URLs] firstObject];
			NSString *path = [selectedDirectoryURL path];
			
			// Utilisez le chemin sélectionné (path) comme nécessaire
			NSLog(@"Selected directory: %@", path);
		}

		return Path();
	}
}
