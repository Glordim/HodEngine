import Cocoa

class SwiftTest
{

}

struct SwiftTest2
{

}

public func printWelcomeMessage(_ name: String) {
  print("Welcome \(name)")
}

public class MacOsNativeWindow
{
    /*
    override func close()
    {
        super.close()
        print("Window Closed")
    }

    func Update()
    {
        /*
        let event: NSEvent? = nextEvent(matching: .any)
        if (event != nil)
        {
            sendEvent(event)
            flushEventQueue()
        }
        */
    }
    */

    public init()
    {

    }

    public func Show()
    {
        _window.makeKeyAndOrderFront(nil);
    }

    public func Maximize()
    {

    }

    public func Center()
    {
        _window.center();
    }

    private let _window: NSWindow = NSWindow(contentRect: .init(origin: .zero,
                                                size: .init(width: NSScreen.main!.frame.midX,
                                                            height: NSScreen.main!.frame.midY)),
                             styleMask: [.closable],
                             backing: .buffered,
                             defer: false)
}