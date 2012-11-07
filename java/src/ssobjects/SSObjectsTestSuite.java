package ssobjects;

import junit.framework.*;

public class SSObjectsTestSuite
{
  /**Run with:
   *
   *    java -cp .;..\classes;..\jars\junit.jar AllMayoMintClasses
   *
   * if you want to just run command line version.
   */
  public static void main(String[] args)
  {
    junit.textui.TestRunner.run(suite());
  }
  public static Test suite()
  {
    TestSuite suite = new TestSuite("All JUnit Tests");
    suite.addTest(ssobjects.PacketBufferTest.suite());
    suite.addTest(ssobjects.PacketMessageQueTest.suite());
    suite.addTest(ssobjects.BufferedSocketTest.suite());
    return suite;
  }
}

