resource "azurerm_virtual_network" "vpc" {
  name                = "vpc"
  address_space       = ["10.1.0.0/16"]
  location            = var.resource_group_location
  resource_group_name = var.resource_group_name
}

resource "azurerm_subnet" "subnet" {
  name                 = "subnet"
  resource_group_name  = var.resource_group_name
  virtual_network_name = azurerm_virtual_network.vpc.name
  address_prefixes     = ["10.1.0.0/24"]
}