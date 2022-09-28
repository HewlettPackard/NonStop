package com.hpe.nsk.nsdevops.server.test;

import org.junit.Assert;
import org.junit.Test;

import com.hpe.nsk.nsdevops.server.JavaHelloServer;
import junit.framework.TestCase;

public class JavaHelloServerTest extends TestCase {

	JavaHelloServer jshObj;
	

	protected void setUp() throws Exception {
		super.setUp();
			
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}
	@Test
	public void testsayHello(){
		try{
		jshObj= new JavaHelloServer("127.0.0.1", 3456);
		jshObj.sayHello("World!");
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	 public static void main(String[] args) {
		try{
		    JavaHelloServerTest test = new JavaHelloServerTest();
		    test.testsayHello();
		   }catch (Exception e) {
          }

	}
}
/*JUNIT 5 TC*/
/*package com.hpe.nsk.nsdevops.server.test;

import static org.junit.jupiter.api.Assertions.assertNotNull;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;
import com.hpe.nsk.nsdevops.server.JavaHelloServer;

class JavaHelloServerTest {
 
    @Test
    void justAnExample() {
 
    }
    
    @Nested
    @DisplayName("When object is not null")
    class WhenObjectIsNotNotNull {
 
        @Test
        @DisplayName("Should not be null")
        void shouldNotBeNull() {
            try{
             JavaHelloServer jshObj = new JavaHelloServer("127.0.0.1", 3456);
             assertNotNull(jshObj.sayHello("World"));
             assertNotNull(jshObj.sayHello("BeautifulWorld"));
            }catch (Exception e) {
          }
        }
    }
}
*/