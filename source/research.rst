Storage Research at Loyola
==========================

Filesystems Research at Loyola
------------------------------

-  Profs. Thiruvathukal, Laufer, and myself have been working on
   filesystems and filesystems frameworks for several years.

-  Our first project was OLFS. OLFS was a FUSE filesystem similar to
   UnionFS, but written in Java. This was an excellent learning
   experience for us.

-  Building on our experience with OLFS, we developed two frameworks:
   NOFS and RestFS. We believe that both of these frameworks help to
   both bring filesystems development to high level languages and
   greatly simplify the problem space.

NOFS
----

-  In most OOP graphical applications, there are a few common
   components:

   -  A domain model

   -  A view/presentation layer

   -  A controller layer that translates between the domain model and
      the presentation layer

   -  A persistence layer. This can be the filesystem or a database.

-  The principle behind the NO part of NOFS (Naked Objects) is that a
   behaviorally complete domain model can be all that an application
   needs if the other layers can be generated in an automated fashion.

-  For many applications the Naked Objects architecture reduces the
   amount of development effort and complexity while producing a decent
   application

NOFS
----

-  The principle behind NOFS is that the fundamental structures or
   domain objects of a filesystem should be all that need to be
   implemented and the interface to any VFS layer or any persistence
   mechanism (hard disk, database, filesystem) should be provided in an
   automatic fashion.

-  With NOFS, it is possible to build a complete filesystem with a
   single Java class containing no more than 50-60 lines of code and
   little or no knowledge about the filesystem contract (operations like
   truncate, read, write, seek, etc...) or the underlying storage
   architecture (hard disk, files, etc...)

An Example NOFS Filesystem in 3 Slides
--------------------------------------

::

    @RootFolderObject
    @DomainObject
    @FolderObject(CanAdd=false, CanRemove=false)
    public class Book {
      private IDomainObjectContainer<Book> _bookContainer;
      private IDomainObjectContainerManager _containerManager;
      @NeedsContainer
      public void setContainer(IDomainObjectContainer<Book> container) {
        _bookContainer = container;
      }
      @NeedsContainerManager
      public void setContainerManager(IDomainObjectContainerManager manager) {
        _containerManager = manager;
      }
      private IDomainObjectContainerManager GetContainerManager() throws Exception {
        if(_containerManager -- null) {
          throw new Exception("container manager is null");
        }
        return _containerManager;
      }

      public List<Contact> getContacts() throws Exception {
        IDomainObjectContainer<Contact> contactContainer =
          GetContainerManager().GetContainer(Contact.class);
        if(contactContainer -- null) {
          throw new Exception("Container for Contact is null");
        }
        return new LinkedList<Contact>(contactContainer.GetAllInstances());
      }

An Example NOFS Filesystem in 3 Slides
--------------------------------------

::

      @Executable
      public void AddAContact(String name, String phone) throws Exception {
        IDomainObjectContainer<Contact> contactContainer = 
                        GetContainerManager().GetContainer(Contact.class);
        Contact contact = contactContainer.NewPersistentInstance();
        contact.setName(name);
        contact.setPhoneNumber(phone);
        _bookContainer.ObjectChanged(this);
      }

      @Executable
      public void RemoveAContact(Contact contact) throws Exception {
        IDomainObjectContainer<Contact> contactContainer = 
                     GetContainerManager().GetContainer(Contact.class);
        contactContainer.Remove(contact);
        _bookContainer.ObjectChanged(this);
      }

      @Executable
      public void RenameAContact(Contact contact, String newName) throws Exception {
        IDomainObjectContainer<Contact> contactContainer = 
                     GetContainerManager().GetContainer(Contact.class);
        String oldName = contact.getName();
        contact.setName(newName);
        contactContainer.ObjectRenamed(contact, oldName, newName);
        contactContainer.ObjectChanged(contact);
      }
    }

An Example NOFS Filesystem in 3 Slides
--------------------------------------

::

    @DomainObject
    public class Contact {
      private String _name;
      private String _phoneNumber;
      private IDomainObjectContainer<Contact> _container;

      @ProvidesName
      public String getName() {
        return _name;
      }

      @ProvidesName
      public void setName(String name) throws Exception {
        _name = name;
        _container.ObjectChanged(this);
      }

      public String getPhoneNumber() { return _phoneNumber; }
      public void setPhoneNumber(String value) throws Exception {
        _phoneNumber = value;
        _container.ObjectChanged(this);
      }

      @NeedsContainer
      public void setContainer(IDomainObjectContainer<Contact> container) {
        _container = container;
      }
    }

NOFS - Running the Sample
-------------------------

    .. figure:: storage/diagrams/nofs_addressbook.*
       :align: center
       :alt: image

       image

NOFS
----

-  With NOFS, the developer can fill in as many details as they are
   concerned about.

-  If the developer wants to implement details about permissions, random
   file access, persistence, caches, etc.. it is optional. Any details
   left out are filled in by the NOFS framework with reasonable
   defaults.

-  For application oriented filesystems - filesystems that are oriented
   more towards behavior than storage - the developer does not need to
   be concerned with any detail of the filesystem contract.

-  For storage oriented filesystems, the developer needs to manage more
   details. Things like permissions, access / modify times, user, and
   group ownership become more relevant.

NOFS - Architecture - Relation to FUSE and the OS
-------------------------------------------------

    .. figure:: storage/diagrams/nofs_kernel_usermode.*
       :align: center
       :alt: image

       image

NOFS - Architecture - Translation of Domain Model to Files
----------------------------------------------------------

    .. figure:: storage/diagrams/nofs_cache_serialization.*
       :align: center
       :alt: image

       image

NOFS - Architecture - Method Execution
--------------------------------------

    .. figure:: storage/diagrams/nofs_execution.*
       :align: center
       :alt: image

       image

RestFS
------

-  While developing NOFS, we realized that there were two common types
   of application oriented filesystems

-  The first type is a local or mixed local / remote behavioral
   filesystem. Something like the addressbook example that we
   demonstrated earlier.

-  The second type that we discovered where filesystems that connected
   to web services. One example we built was a NOFS filesystem in about
   400-500 lines of code to mount a Flickr photo album as a folder of
   pictures.

-  With the knowledge that RESTful services are very similar to
   filesystems architecturally and how well NOFS was doing with
   application oriented filesystems. We asked ourselves if we could make
   accessing web services even easier.

RestFS - Service Composition
----------------------------

    .. figure:: storage/diagrams/composition.*
       :align: center
       :alt: image

       image

RestFS
------

-  While developing application filesystems for web services with NOFS,
   we found that domain model modifications were roughly mapping one or
   two FUSE operations to a HTTP verb.

-  So, our first attempt with RestFS has been to provide file and
   configuration file pairs to map particular FS system calls onto
   particular web methods.

-  This has allowed us to do things such as configure a file so that
   when it is ’touched’ that a Google search can be performed with the
   results stored to the file.

RestFS - Communications
-----------------------

    .. figure:: storage/diagrams/communication_timeline.*
       :align: center
       :alt: image

       image

RestFS - Google Search
----------------------

::

    #!/bin/bash
    URL_ARGS=`echo $@ | sed 's/ /%20/g'`
    FILE_NAME=`echo $@ | sed 's/ /_/g'`
    RESOURCE="ajax/services/search/web?v=1.0&q=$URL_ARGS"
    RESOURCE=`echo $RESOURCE | \
             sed -e 's~&~\&amp;~g' \
                 -e 's~<~\&lt;~g' \
                 -e 's~>~\&gt;~g'`
    touch $FILE_NAME
    ./configureResource .$FILE_NAME \
             fs:utime web:get \
             host:ajax.googleapis.com \
             resource:$RESOURCE
    touch $FILE_NAME
    cat $FILE_NAME

RestFS - Authentication
-----------------------

-  A common concern for many rest-ful services is authentication.

-  A popular authentication method for many rest-ful services is OAuth.

-  Since RestFS is built on top of NOFS, we were able to map an existing
   OAuth library into the filesystem

-  In RestFS, there is a special OAuth folder called ’/auth’

RestFS - Authentication
-----------------------

    .. figure:: storage/diagrams/restfs_auth.*
       :align: center
       :alt: image

       image

RestFS - Authentication
-----------------------

    .. figure:: storage/diagrams/restfs_authentication_process.*
       :align: center
       :alt: image

       image

RestFS
------

-  With RestFS we’ve been able to show how local software that works
   principally with files (cat, grep), can be used with web services

-  We’ve also been able to demonstrate how remote web services can be
   enhanced locally with RestFS and then re-exposed as new rest-ful web
   services.

RestFS - Future Directions
--------------------------

-  We’re currently re-architecting RestFS to be more rules based so that
   more than one FS system call can be mapped per file.

-  We’re also investigating how folders and symbolic links can be used
   to further enhance RestFS.

-  During the fall of 2011, with the help of Shaohui Chen, we’ve been
   able to move NOFS from Java to the .NET framework

-  This porting effort will allow us to support both Windows and Linux,
   and give us a much better language framework to work from.

