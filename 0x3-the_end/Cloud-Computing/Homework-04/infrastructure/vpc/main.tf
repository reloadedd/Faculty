resource "azurerm_virtual_network" "vpc" {
  name                = "vpc1"
  address_space       = ["10.1.0.0/24"]
  location            = var.resource_group_location
  resource_group_name = var.resource_group_name
}

resource "azurerm_subnet" "subnet1" {
  name                 = "subnet1"
  resource_group_name  = var.resource_group_name
  virtual_network_name = azurerm_virtual_network.vpc.name
  address_prefixes     = ["10.1.0.0/26"]
}