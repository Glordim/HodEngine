//import Cocoa
import HodMain

print("Bonjour, monde!")
/*
// Créer une fenêtre
let frame = NSRect(x: 0, y: 0, width: 800, height: 600)
let window = NSWindow(contentRect: frame,
                      styleMask: [.titled, .closable, .miniaturizable, .resizable],
                      backing: .buffered,
                      defer: false)
window.title = "Ma fenêtre"
window.makeKeyAndOrderFront(nil)
window.center()
window.level = .floating // Assure que la fenêtre est au premier plan
// Lancer l'application
NSApp.run()
*/
let values: UnsafePointer<UnsafePointer<CChar>?> = 
        UnsafeRawPointer(CommandLine.unsafeArgv).bindMemory(
            to: UnsafePointer<CChar>?.self,
            capacity: Int(CommandLine.argc)
        )

HodMain(CommandLine.argc, CommandLine.unsafeArgv)
