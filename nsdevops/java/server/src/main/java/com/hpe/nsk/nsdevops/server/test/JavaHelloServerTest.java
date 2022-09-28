/*
 (C) Copyright 2020 Hewlett Packard Enterprise Development LP

 This software is provided subject to the terms of your
 existing license agreement with Hewlett Packard Enterprise
 Development LP or its reseller, integrator,
 distributor or other authorized sub-licensor for the
 use of Licensed Products or Software supplied by
 Hewlett Packard Enterprise Development LP .
*/
package com.hpe.nsk.nsdevops.server.test;

import static org.junit.jupiter.api.Assertions.assertNotNull;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Nested;
import com.hpe.nsk.nsdevops.server.JavaHelloServer;

/**
  *JavaHelloServerTest is a junit test to demonstrate CI/CD on Nonstop.
  */
class JavaHelloServerTest {
/** Dummy Test.
  */
    @Test
    void justAnExample() {

    }
/** Dummy Test.
  */
    @Nested
    @DisplayName("When object is not null")
    class WhenObjectIsNotNotNull {
    /** Test Case shouldNotBeNull.
      * Checks if the response is not null
      */
        @Test
        @DisplayName("Should not be null")
        void shouldNotBeNull() {
            final int port = 3456;
            try {
             JavaHelloServer jshObj = new JavaHelloServer("127.0.0.1", port);
             assertNotNull(jshObj.sayHello("World"));
             assertNotNull(jshObj.sayHello("BeautifulWorld"));
            } catch (Exception e) {
          }
        }
    }
}
