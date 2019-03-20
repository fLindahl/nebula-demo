using System;
using System.Runtime.InteropServices;

public class HelloWorld
{
    [DllImport ("__Internal", EntryPoint="Foobar")]
    static extern void Foobar ();

    static public void Main ()
    {
        Console.WriteLine ("Hello Mono World");
        Foobar();
    }
}